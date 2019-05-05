cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(meetupcpp2019 CXX)

# Always enable generation of compilation database to
# make clang tooling and vscode happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generate compile commands database" FORCE)

set(MEETUPCPP_DEFAULT_WARNINGS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror -Wall -Wextra -pedantic-errors>
    $<$<CXX_COMPILER_ID:MSVC>:
        W4>
)

set(TRISYCL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/triSYCL/include CACHE PATH "" FORCE)
include(external/triSYCL/cmake/FindTriSYCL.cmake)

add_executable(vector_add_tmp_sycl_scalar vector_add_tmp_sycl_scalar.cpp)
add_sycl_to_target(vector_add_tmp_sycl_scalar)

add_executable(vector_add_tmp_sycl vector_add_tmp_sycl.cpp)
add_sycl_to_target(vector_add_tmp_sycl)