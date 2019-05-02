#include <CL/sycl.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace cl;

template<typename T>
struct add_kernel_tag{};

template <typename T>
std::vector<T> add(const std::vector<T>& a, const std::vector<T>& b) {
    // We are going to operate on the common indexes subset
    const auto min_size = std::min(std::size(a), std::size(b));
    if(min_size <= 0) return {};

    std::vector<T> result(min_size);

    {   // Open a new scope to bind the lifetime of the command queue.
        // Queue's destructor will wait for all pending operations to complete.
        sycl::queue queue;

        // Create buffers (views on a, b and result contiguous storage)
        sycl::buffer<T> A{std::begin(a), std::begin(a) + n_elements};
        sycl::buffer<T> B{std::begin(b), std::begin(b) + n_elements};
        sycl::buffer<T> R{std::begin(result), std::end(result)};

        // The command group describing all operations needed for the kernel
        // execution
        queue.submit([&](sycl::handler& cgh) {
            // Get proper accessors to existing buffers by specifying
            // read/write intents
            auto ka = A.get_access<sycl::access::mode::read>(cgh);
            auto kb = B.get_access<sycl::access::mode::read>(cgh);
            auto kr = R.get_access<sycl::access::mode::write>(cgh);

            // Enqueue a single, scalar task
            cgh.single_task<add_kernel_tag<T>>([=]() {  // Be sure to capture by value!
                // for (size_t i = 0; i < min_size; ++i) kc[i] = ka[i] + kb[i];
                std::transform(
                        std::begin(ka), std::end(ka),
                        std::begin(kb),
                        std::begin(kresult),
                        std::plus<T>{});
            });
        });  // End of our commands for this queue
    }        // End scope, so we wait for the queue to complete
    // Queue destructor has completed, results are now available
    // in output storage
    return result;
}

template<typename T>
std::vector<T> make_dataset(std::size_t size) {
    std::vector<T> result;
    // Fill with garbage...
    return result;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <items count>" << std::endl;
        return 1;
    }
    const auto count = std::stoll(argv[1]);
    using element_type = float;
    const auto a = make_dataset<element_type>(count);
    const auto b = make_dataset<element_type>(count);
    const auto result = add(a, b);
    std::cout << "Result:" << std::endl;
    for (size_t i = 0; i != N; i++) std::cout << c[i] << " ";
    std::cout << std::endl;
}
