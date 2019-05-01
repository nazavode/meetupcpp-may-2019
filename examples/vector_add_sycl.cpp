#include <CL/sycl.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cl;

template<typename T>
struct add_kernel_tag{};

template <typename T>
std::vector<T> add(const std::vector<T>& a, const std::vector<T>& b) {
    // Contract:
    assert(std::size(a) == std::size(b));

    std::vector<T> result(std::size(a), {});

    {  
        // By sticking all the SYCL work in a {} block, we ensure
        // all SYCL tasks must complete before exiting the block

        // Create a command queue to work on
        sycl::queue queue;

        // Create buffers from a & b vectors with 2 different syntax
        sycl::buffer<T> A{std::begin(a), std::end(a)};
        sycl::buffer<T> B{std::begin(b), std::end(b)};

        // A buffer of N float using the storage of c
        sycl::buffer<T> C{std::begin(result), std::end(result)};

        // The command group describing all operations needed for the kernel
        // execution
        queue.submit([&](sycl::handler& cgh) {
            // In the kernel A and B are read, but C is written
            auto ka = A.get_access<sycl::access::mode::read>(cgh);
            auto kb = B.get_access<sycl::access::mode::read>(cgh);
            auto kc = C.get_access<sycl::access::mode::write>(cgh);

            // Enqueue a single, simple task
            cgh.single_task<add_kernel_tag<T>>([=]() {
                for (size_t i = 0; i != N; i++) kc[i] = ka[i] + kb[i];
            });
        });  // End of our commands for this queue
    }        // End scope, so we wait for the queue to complete
    // Queue destructor has been run, results are now available
    // in the output storage location
    return result;
}

template<typename T>
std::vector<T> make_dataset(std::size_t size) {
    std::vector<T> result;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <items count>" << std::endl;
        return 1;
    }
    const auto count = std::stoll(argv[1]);
    using element_type = float;
    std::vector<element_type>

    std::cout << "Result:" << std::endl;
    for (size_t i = 0; i != N; i++) std::cout << c[i] << " ";
    std::cout << std::endl;
}
