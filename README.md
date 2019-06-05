# Look ma, no CUDA! Programming GPUs with modern C++

This repo contains all the stuff used during the talk I gave at the
[Italian C++ Community](https://www.italiancpp.org/) meetup in 
[Modena, 2019-05-09](https://www.italiancpp.org/event/meetup-maggio2019/).
Please feel free to steal everything you want.

## Contents

Please note that some submodules are needed to have all the stuff working properly, be
sure to `git clone --recurse-submodules`.

| Path        | What to expect |
| ----------- | ------------------------------------------- |
| `slides/`   | The slide deck. Nothing more, nothing less. |
| `examples/` | Show me the code. |
| `cmake/`    | CMake helpers, mainly intended to support [ComputeCpp](https://www.codeplay.com/products/computesuite/computecpp) and directly copied from its distribution. |
| `external/` | All `git` submodules related to external dependencies. |

## Getting Started

| Example | Depends on |
| ------- | ---------- |
| [vector_add_sycl.cpp](examples/vector_add_sycl.cpp) | SYCL |
| [vector_add_sycl_serial.cpp](examples/vector_add_sycl_serial.cpp) | SYCL |
| [vector_add_tmp_sycl.cpp](examples/vector_add_tmp_sycl.cpp) | SYCL |
| [vector_add_tmp_sycl_serial.cpp](examples/vector_add_tmp_sycl_serial.cpp)| SYCL |
| [vector_add_opencl.cpp](examples/vector_add_opencl.cpp)| OpenCL |
| [vector_add_cuda.cu](examples/vector_add_cuda.cu) | CUDA |
| [matrix_add_sycl.cpp](examples/matrix_add_sycl.cpp)| SYCL |

The examples in this repo have been tested with the following SYCL implementations:

 * [triSYCL](https://github.com/triSYCL/triSYCL), the open source, standard reference implementation;
 * [ComputeCpp](https://www.codeplay.com/products/computesuite/computecpp), a commercial implementation by [codeplay](https://www.codeplay.com/) with a freely available community edition.

### SYCL and OpenCL Examples

In order to be able to build the examples based on SYCL you

### CUDA Examples

## Credits

## Resources

* [Accelerating your C++ on GPU with SYCL by Simon Brand](https://blog.tartanllama.xyz/sycl/) - one of the nicest introductions around.
* [Programming GPUs with SYCL by Gordon Brown](http://cppedinburgh.uk/slides/201607-sycl.pdf)
* [SYCL Developer Guide by Codeplay](https://developer.codeplay.com/products/computecpp/ce/guides/sycl-guide?)
* [CppCon 2018: Parallel Programming with Modern C++ by Gordon Brown](https://github.com/AerialMantis/cppcon2018-parallelism-class)
* [Modern C++ for accelerators: a SYCL deep dive by Andrew Richards](https://www.khronos.org/assets/uploads/developers/library/2018-evs/EVS2018_09_Modern_Cpp_for_accelerators_andrew.pdf)
* [2019 EuroLLVM Developers’ Meeting: A. Savonichev (Intel) "SYCL compiler: zero-cost abstraction and type safety for heterogeneous computing"](https://youtu.be/rfg19iODkhI)
* [SYCL 1.2.1 API Reference Card](https://www.khronos.org/files/sycl/sycl-12-reference-card.pdf) - print and hang it on the wall next to the Picasso you just bought.
* [SYCL Standard Specification](https://www.khronos.org/registry/SYCL/) - to be a proper standard, you need a proper spec. Not so *standardese* (actually quite educational) but definitely not a novel.
