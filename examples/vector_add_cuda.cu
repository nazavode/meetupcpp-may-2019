#include <algorithm>
#include <cstddef>
#include <vector>

__global__ void add_kernel(const int* a, const int* b, int* result, int count) {
    // Get our global thread ID
    int id = blockIdx.x * blockDim.x + threadIdx.x;

    // Make sure we do not go out of bounds
    if (id < count) {
        c[id] = a[id] + b[id];
    }
}

std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b) {
    // We are going to operate on the common indexes subset
    std::vector<int> result(std::min(std::size(a), std::size(b)));
    if (std::size(result) == 0) return {};
    const auto byte_size = std::size(result) * sizeof(int);

    // Add some proper RAII to cuda resources
    const auto make_unique_device =
        [](std::size_t size) -> std::unique_ptr<int[], decltype(cudaFree)> {
        void* ptr;
        cudaMalloc(&ptr, size);
        return {ptr, cudaFree};
    };

    // Device allocation
    auto device_a = make_device_buffer(byte_size);
    auto device_b = make_device_buffer(byte_size);
    auto device_result = make_device_buffer(byte_size);

    // Copy host vectors to device
    cudaMemcpy(device_a.get(), std::data(host_a), byte_size, cudaMemcpyHostToDevice);
    cudaMemcpy(device_b.get(), std::data(host_b), byte_size, cudaMemcpyHostToDevice);

    int blockSize, gridSize;

    // Number of threads in each thread block
    blockSize = 1024;

    // Number of thread blocks in grid
    gridSize = (int)ceil((float)n / blockSize);

    // Execute the kernel
    add_kernel<<<gridSize, blockSize>>>(device_a.get(), device_b.get(),
                                        device_result.get(), count);

    // Copy result back to host
    cudaMemcpy(std::data(host_result), device_result.get(), byte_size,
               cudaMemcpyDeviceToHost);

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