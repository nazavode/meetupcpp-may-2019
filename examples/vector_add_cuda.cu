#include <algorithm>
#include <cstddef>
#include <vector>
#include <iterator>
#include <string>
#include <iostream>

__global__ void add_kernel(const int* a, const int* b, int* result, int count) {
    // Get our global thread ID
    int id = blockIdx.x * blockDim.x + threadIdx.x;

    // Check for out of bounds threads
    if (id < count) {
        result[id] = a[id] + b[id];
    }
}

std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b) {
    // We are going to operate on the common indexes subset
    std::vector<int> result(std::min(a.size(), b.size()));
    if (result.size() == 0) return {};

    const auto byte_size = result.size() * sizeof(int);

    int* device_a = nullptr;
    int* device_b = nullptr;
    int* device_result = nullptr;

    // Device allocation
    cudaMalloc(&device_a, byte_size);
    cudaMalloc(&device_b, byte_size);
    cudaMalloc(&device_result, byte_size);

    // Copy host vectors to device
    cudaMemcpy(device_a, a.data(), byte_size, cudaMemcpyHostToDevice);
    cudaMemcpy(device_b, b.data(), byte_size, cudaMemcpyHostToDevice);

    // Number of threads in each thread block
    const auto blockSize = 1024;

    // Number of thread blocks in grid
    const auto gridSize = static_cast<int>(
        std::ceil(static_cast<float>(result.size()) / blockSize)
    );

    // Execute the kernel
    add_kernel<<<gridSize, blockSize>>>(device_a, device_b,
                                        device_result, result.size());

    // Copy result back to host
    cudaMemcpy(result.data(), device_result, result.size() * sizeof(int),
               cudaMemcpyDeviceToHost);

    cudaFree(device_a);
    cudaFree(device_b);
    cudaFree(device_result);

    return result;
}

std::vector<int> make_dataset(std::size_t size) {
    std::vector<int> dataset(size);
    std::generate(std::begin(dataset), std::end(dataset), [&]() {
        static int value = 0;
        return value++;
    });
    std::random_shuffle(std::begin(dataset), std::end(dataset));
    return dataset;
}

int main(int argc, char* argv[]) {
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
                   std::plus<int>{});
    const auto diff =
        std::mismatch(std::begin(result), std::end(result), std::begin(expected),
                      [](auto a, auto b) { return a == b; });
    const auto result_it = diff.first;
    const auto expected_it = diff.second;
    if (result_it != std::end(result)) {
        const auto diff_idx = std::distance(std::begin(result), result_it);
        std::cerr << "diff at index " << diff_idx << ": " << *result_it
                  << " != " << *expected_it << std::endl;
        return 1;
    }
}
