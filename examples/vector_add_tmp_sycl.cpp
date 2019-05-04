#include <CL/sycl.hpp>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

using namespace cl;

template <typename T>
struct AddKernel {};

template <typename T>
std::vector<T> add(const std::vector<T>& a, const std::vector<T>& b) {
    // We are going to operate on the common indexes subset
    std::vector<T> result(std::min(std::size(a), std::size(b)));
    if (result.size() == 0) return {};

    {  // Open a new scope to bind the lifetime of the command queue.
        // Queue's destructor will wait for all pending operations to complete.
        sycl::queue queue;

        // Create buffers (views on a, b and result contiguous storage)
        sycl::buffer<T> A{std::data(a), std::size(result)};
        sycl::buffer<T> B{std::data(b), std::size(result)};
        sycl::buffer<T> R{std::data(result), std::size(result)};

        // The command group describing all operations needed for the kernel
        // execution
        queue.submit([&](sycl::handler& cgh) {
            // Get proper accessors to existing buffers by specifying
            // read/write intents
            auto ka = A.template get_access<sycl::access::mode::read>(cgh);
            auto kb = B.template get_access<sycl::access::mode::read>(cgh);
            auto kr = R.template get_access<sycl::access::mode::write>(cgh);

            // Enqueue a single, scalar task
            cgh.single_task<AddKernel<T>>([=]() {  // Be sure to capture by value!
                std::transform(std::begin(ka), std::end(ka), std::begin(kb),
                               std::begin(kr), std::plus<T>{});
            });
        });  // End of our commands for this queue
    }        // End scope, so we wait for the queue to complete
    // Queue destructor has completed, results are now available
    // in output storage
    return result;
}

template <typename T>
std::vector<T> make_dataset(std::size_t size) {
    static auto distribution = [] {
        if constexpr (std::is_floating_point_v<T>) {
            return std::uniform_real_distribution<T>{std::numeric_limits<T>::min(),
                                                     std::numeric_limits<T>::max() / 2};
        } else {
            return std::uniform_int_distribution<T>{std::numeric_limits<T>::min(),
                                                    std::numeric_limits<T>::max() / 2};
        }
    }();
    static std::default_random_engine generator;
    std::vector<T> dataset(size);
    std::generate(std::begin(dataset), std::end(dataset),
                  [&]() { return distribution(generator); });
    return dataset;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <items count>" << std::endl;
        return 1;
    }
    const auto count = std::stoll(argv[1]);
    using element_type = int;
    const auto a = make_dataset<element_type>(count);
    const auto b = make_dataset<element_type>(count);
    const auto result = add(a, b);
    // Check correctness
    const auto equals = [](element_type lhs, element_type rhs) {
        if constexpr (std::is_floating_point_v<element_type>) {
            // Quick and dirty, don't try this at home.
            // If you really want to compare floating points in the real world, have a
            // look here: https://www.itu.dk/~sestoft/bachelor/IEEE754_article.pdf
            return std::abs(lhs - rhs) < std::numeric_limits<element_type>::epsilon();
        } else {
            return lhs == rhs;
        }
    };
    for (decltype(std::size(result)) i = 0; i < std::size(result); ++i) {
        if (!equals(result[i], a[i] + b[i])) {
            std::cerr << "diff at index " << i << ": " << result[i]
                      << " != " << a[i] + b[i] << std::endl;
            return 1;
        }
    }
}
