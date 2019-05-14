#include <algorithm>
#include <functional>
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <cstddef>

#define __CL_ENABLE_EXCEPTIONS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

static const char* addKernelSource =
    "__kernel void vecAdd(  __global int *a,                      \n"
    "                       __global int *b,                      \n"
    "                       __global int *c,                      \n"
    "                       const unsigned int n)                 \n"
    "{                                                            \n"
    "    //Get our global thread ID                               \n"
    "    const unsigned int id = (unsigned int) get_global_id(0); \n"
    "                                                             \n"
    "    //Make sure we do not go out of bounds                   \n"
    "    if (id < n)                                              \n"
    "        c[id] = a[id] + b[id];                               \n"
    "}                                                            \n";

std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b) {
    // We are going to operate on the common indexes subset
    std::vector<int> result(std::min(std::size(a), std::size(b)));
    if (std::size(result) == 0) return {};
    const auto size_bytes = std::size(result) * sizeof(int);

    // Query available platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.size() == 0) {
        throw std::runtime_error{"no OpenCL platforms available"};
    }

    // Get list of devices on default platform and create context
    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM,
                                          (cl_context_properties)(platforms[0])(), 0};
    cl::Context context{CL_DEVICE_TYPE_GPU, properties};
    std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

    // Create command queue for first device
    auto err = CL_SUCCESS;
    cl::CommandQueue queue{context, devices[0], 0, &err};

    // Create device memory buffers
    auto device_a = cl::Buffer{context, CL_MEM_READ_ONLY, size_bytes};
    auto device_b = cl::Buffer{context, CL_MEM_READ_ONLY, size_bytes};
    auto device_r = cl::Buffer{context, CL_MEM_WRITE_ONLY, size_bytes};

    // Bind memory buffers
    queue.enqueueWriteBuffer(device_a, CL_TRUE, 0, size_bytes, std::data(a));
    queue.enqueueWriteBuffer(device_b, CL_TRUE, 0, size_bytes, std::data(b));

    // Build kernel from source string
    cl::Program::Sources source{1, std::make_pair(addKernelSource, std::strlen(addKernelSource))};
    cl::Program program = cl::Program{context, source};
    program.build(devices);

    // Create kernel object
    cl::Kernel kernel{program, "vecAdd", &err};

    // Bind kernel arguments to kernel
    kernel.setArg(0, device_a);
    kernel.setArg(1, device_b);
    kernel.setArg(2, device_r);
    kernel.setArg(3, static_cast<cl_uint>(std::size(result)));

    // Number of work items in each local work group
    cl::NDRange localSize{64};
    // Number of total work items - localSize must be devisor
    cl::NDRange globalSize{
        static_cast<std::size_t>(std::ceil(std::size(result) / static_cast<float>(64)) * 64)};

    // Enqueue kernel
    cl::Event event;
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, localSize, NULL,
                               &event);

    // Block until kernel completion
    event.wait();

    // Read back result
    queue.enqueueReadBuffer(device_r, CL_TRUE, 0, size_bytes,
                            std::data(result));
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
