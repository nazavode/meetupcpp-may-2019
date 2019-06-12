#if defined(TRISYCL_OPENCL) && defined(__APPLE__)
// TODO: OpenCL header on macOSX shows some weird macro explosion
// during the expansion of the CL_DEPRECATED decorator macro that
// should be investigated further. Just elide it for now.
#define CL_SILENCE_DEPRECATION 1
#endif
#include <CL/sycl.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

using namespace cl;

// Kernel type tag, be sure to declare it (forward is fine)
// in an externally accessible namespace (e.g.: no anonymous)
class AddKernel;

std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b) {
    // We are going to operate on the common indexes subset
    std::vector<int> result(std::min(std::size(a), std::size(b)));
    if (std::size(result) == 0) return {};

    // Open a new scope to bind the lifetime of the command queue.
    {
        // Queue's destructor will wait for all pending operations to complete.
        sycl::queue queue;

        // Create buffers (views on a, b and result contiguous storage)
        sycl::buffer<int> A{std::data(a), std::size(result)};
        sycl::buffer<int> B{std::data(b), std::size(result)};
        sycl::buffer<int> R{std::data(result), std::size(result)};

        // The command group describing all operations needed for the kernel
        // execution
        queue.submit([&](sycl::handler& cgh) {
            // Get proper accessors to existing buffers by specifying
            // read/write intents
            auto ka = A.get_access<sycl::access::mode::read>(cgh);
            auto kb = B.get_access<sycl::access::mode::read>(cgh);
            auto kr = R.get_access<sycl::access::mode::write>(cgh);

            // Enqueue parallel kernel
            cgh.parallel_for<AddKernel>(
                sycl::range<1>{std::size(result)},
                [=](sycl::id<1> idx) {  // Be sure to capture by value!
                    kr[idx] = ka[idx] + kb[idx];
                });
        });  // End of our commands for this queue
    }        // End scope, so we wait for the queue to complete
    // Queue destructor has completed, results are now available
    // in output storage
    return result;
}

std::vector<int> make_dataset(std::size_t size) {
    std::vector<int> dataset(size);
    std::iota(std::begin(dataset), std::end(dataset), 0);
    std::random_shuffle(std::begin(dataset), std::end(dataset));
    return dataset;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <items count>" << std::endl;
        return 1;
    }
    const auto count = std::stoll(argv[1]);

    const auto a = make_dataset(count);
    const auto b = make_dataset(count);
    const auto result = add(a, b);
    // Correctness check
    auto expected = std::vector<int>(count);
    std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(expected),
                   std::plus<>{});
    auto [result_it, expected_it] =
        std::mismatch(std::begin(result), std::end(result), std::begin(expected),
                      [](auto a, auto b) { return a == b; });
    if (result_it != std::end(result)) {
        const auto diff_idx = std::distance(std::begin(result), result_it);
        std::cerr << "diff at index " << diff_idx << ": " << *result_it
                  << " != " << *expected_it << std::endl;
        return 1;
    }
}
