# Look ma, no CUDA! Programming GPUs with modern C++

This repo contains all the stuff used during the talk I gave at the
[Italian C++ Community](https://www.italiancpp.org/) meetup in 
[Modena, 2019-05-09](https://www.italiancpp.org/event/meetup-maggio2019/)
about GPUs and how [SYCL](https://en.wikipedia.org/wiki/SYCL) allows us
to finally program them in a sane way.
Please feel free to steal anything you want.

## Contents

| Path        | What to expect |
| ----------- | ------------------------------------------- |
| `slides/`   | The slide deck. Nothing more, nothing less. |
| `examples/` | Show me the code. |
| `cmake/`    | CMake helpers, mainly intended to support [ComputeCpp](https://www.codeplay.com/products/computesuite/computecpp) and directly copied from its distribution. |
| `external/` | All `git` submodules related to external dependencies. |

## Getting Started

The [`CMakeLists.txt`](CMakeLists.txt) provided in this repo builds the following examples
as separate executables:

| Example | Depends on | WAT? |
| ------- | ---------- | ---- |
| [`vector_add_sycl.cpp`](examples/vector_add_sycl.cpp) | `SYCL` | Parallel vector addition with linear accessors |
| [`vector_add_sycl_serial.cpp`](examples/vector_add_sycl_serial.cpp) | `SYCL` | Serial vector addition with linear accessors | 
| [`vector_add_tmp_sycl.cpp`](examples/vector_add_tmp_sycl.cpp) | `SYCL` | Templated (on value type) parallel vector addition with linear accessors |
| [`vector_add_tmp_sycl_serial.cpp`](examples/vector_add_tmp_sycl_serial.cpp)| `SYCL` | Templated (on value type) serial vector addition with linear accessors |
| [`matrix_add_sycl.cpp`](examples/matrix_add_sycl.cpp)| `SYCL` | Matrix (2-dimensions) parallel addition with 2-dimensional accessors |
| [`vector_add_opencl.cpp`](examples/vector_add_opencl.cpp)| `OpenCL` | Parallel vector addition using a linear kernel grid |
| [`vector_add_cuda.cu`](examples/vector_add_cuda.cu) | `CUDA` | Parallel vector addition using a linear kernel grid |

`OpenCL` and `CUDA` examples are built only if a suitable implementation can be found on the host 
while `SYCL` based ones are built anyway.

All of the `SYCL` based examples have been tested with the following implementations:

 * [`triSYCL`](https://github.com/triSYCL/triSYCL), the open source, standard reference implementation.
   **Depends on [boost](https://www.boost.org/) and a `C++` with
   [`OpenMP` support](https://www.openmp.org/resources/openmp-compilers-tools/)**;
 * [`ComputeCpp`](https://www.codeplay.com/products/computesuite/computecpp), a commercial implementation
   by [codeplay](https://www.codeplay.com/) with a freely available community edition.
 
 Please note that if `SYCL` cannot be found on the host, the `triSYCL` source tree will be
 automatically downloaded and included via `git` submodule.
 

### Build configuration

The following `CMake` options are available:

| Option | Default | Meaning |
| ----------------------- | ------------------------- | -------------------------------------------------------------------------------------------------- |
| `MEETUPCPP_SYCL`        | `triSYCL`                 | SYCL implementation to be used; allowed values are: `triSYCL`, `ComputeCpp`                        |
| `MEETUPCPP_TRISYCL_DIR` | `<repo>/external/triSYCL` | Path of the `triSYCL` source to be used when `triSYCL` is selected; defaults to vendored submodule |
| `COMPUTECPP_BITCODE`    | `ptx64`                   | Bitcode generation target for ComputeCpp; defaults to NVIDIA PTX                                   |
| `TRISYCL_OPENCL`        | `ON`                      | Use `OpenCL` execution backend; ignored when `MEETUPCPP_SYCL` isn't set to `triSYCL` or `OpenCL` isn't available |

## Credits

A lot of excellent charts and pitches have been taken directly from publicly available
talks across the net, I've strived really hard to put proper credits on the slides but
if you notice that something's missing please open an issue on this repo.

Thanks everyone for your help in making my slides more clear and understandable.
I'm particularly grateful to (in order of appearance):

* [Karl Rupp](https://www.karlrupp.net/)
* [Michael Wong](https://wongmichael.com/about/)
* [Gordon Brown](http://www.aerialmantis.co.uk/)
* [Aksel Alpay](https://github.com/illuhad)

## Resources

* [`sycl.tech`](http://sycl.tech/): a comprehensive community platform that collects articles, blog posts, talks and everything related to SYCL that shows up on the web.
* [Accelerating your C++ on GPU with SYCL by Simon Brand](https://blog.tartanllama.xyz/sycl/) - one of the nicest introductions around.
* [Programming GPUs with SYCL by Gordon Brown](http://cppedinburgh.uk/slides/201607-sycl.pdf) - a great introduction to the whys and hows of SYCL.
* [SYCL Developer Guide by Codeplay](https://developer.codeplay.com/products/computecpp/ce/guides/sycl-guide?) - the current lack of learning learning material about SYCL is appalling but this terse developer guide makes the situation a little better.
* [CppCon 2018: Parallel Programming with Modern C++ by Gordon Brown](https://github.com/AerialMantis/cppcon2018-parallelism-class) - a great overview of parallel programming and modern C++.
* [Modern C++ for accelerators: a SYCL deep dive by Andrew Richards](https://www.khronos.org/assets/uploads/developers/library/2018-evs/EVS2018_09_Modern_Cpp_for_accelerators_andrew.pdf) - an excellent slide deck to be printed and studied.
* [2019 EuroLLVM Developers’ Meeting: A. Savonichev (Intel) "SYCL compiler: zero-cost abstraction and type safety for heterogeneous computing"](https://youtu.be/rfg19iODkhI) - a nice insight on how Intel is working on his own SYCL implementation for LLVM; mandatory for compiler nerds.
* [SYCL 1.2.1 API Reference Card](https://www.khronos.org/files/sycl/sycl-12-reference-card.pdf) - print and hang it on the wall next to the Picasso you just bought at Sotheby's.
* [SYCL Standard Specification](https://www.khronos.org/registry/SYCL/) - to be a proper standard, you need a proper spec. Not so *standardese* (a traightforward and educational read, actually) but definitely not a novel.
* [GCC support for offloading to PTX via OpenACC](https://gcc.gnu.org/wiki/Offloading)
* [Compiling CUDA with clang - LLVM 9 documentation](https://llvm.org/docs/CompileCudaWithLLVM.html)
* [CppCon 2016: CUDA is a low-level language by Justin Lebar](https://youtu.be/KHa-OSrZPGo)
* [`gpucc`: An Open-Source GPGPU Compiler](https://ai.google/research/pubs/pub45226)
