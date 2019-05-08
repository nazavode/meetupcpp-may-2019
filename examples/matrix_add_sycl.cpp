#include <CL/sycl.hpp>
#include <cstddef>
#include <iostream>

using namespace cl;

// Size of the matrices
constexpr std::size_t N = 2000;
constexpr std::size_t M = 3000;

int main() {
    // Create a queue to work on default device
    sycl::queue q;
    // Create some 2D buffers with N×M float values for our matrices
    sycl::buffer<double, 2> a{{N, M}};
    sycl::buffer<double, 2> b{{N, M}};
    sycl::buffer<double, 2> c{{N, M}};
    // First launch an asynchronous kernel to initialize buffer "a"
    q.submit([&](sycl::handler &cgh) {
        // The kernel writes "a", so get a write accessor to it
        auto A = a.get_access<sycl::access::mode::write>(cgh);

        // Enqueue parallel kernel on an N×M 2D iteration space
        cgh.parallel_for<class InitAKernel>(sycl::range<2>{N, M}, [=](sycl::item<2> idx) {
            A[idx] = idx[0] * 2 + idx[1];
        });
    });
    // Launch an asynchronous kernel to initialize buffer "b"
    q.submit([&](sycl::handler &cgh) {
        // The kernel writes to "b", so get a write accessor on it
        auto B = b.get_access<sycl::access::mode::write>(cgh);
        // Enqueue a parallel kernel on an N×M 2D iteration space
        cgh.parallel_for<class InitBKernel>(sycl::range<2>{N, M}, [=](sycl::item<2> idx) {
            B[idx] = idx[0] * 2014 + idx[1] * 42;
        });
    });
    // Launch an asynchronous kernel to compute matrix addition c = a + b
    q.submit([&](sycl::handler &cgh) {
        // In the kernel "a" and "b" are read, but "c" is written.
        // Since the kernel reads "a" and "b", the runtime will implicitly add
        // a producer-consumer dependency to the previous kernels producing them.
        auto A = a.get_access<sycl::access::mode::read>(cgh);
        auto B = b.get_access<sycl::access::mode::read>(cgh);
        auto C = c.get_access<sycl::access::mode::write>(cgh);

        // Enqueue a parallel kernel on an N×M 2D iteration space
        cgh.parallel_for<class MatrixAddKernel>(
            sycl::range<2>{N, M}, [=](sycl::item<2> idx) { C[idx] = A[idx] + B[idx]; });
    });
    // Request an access to read "c" from the host-side. The SYCL runtime
    // will wait for "c" to be ready available on the host side before
    // returning the accessor.
    // This means that there is no communication happening in the nested loop below.
    auto C = c.get_access<sycl::access::mode::read>();

    // Correctness check
    for (std::size_t i = 0; i < N; i++)
        for (std::size_t j = 0; j < M; j++)
            // Compare the result to the analytic value
            if (C[i][j] != i * (2 + 2014) + j * (1 + 42)) {
                std::cout << "Wrong value " << C[i][j] << " on element " << i << ' ' << j
                          << '\n';
                exit(EXIT_FAILURE);
            }
}
