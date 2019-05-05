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

# Enable vendored triSYCL
# This will define the unary function: add_sycl_to_target(<target>)
set(TRISYCL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples/external/triSYCL/include CACHE PATH "" FORCE)
include(examples/external/triSYCL/cmake/FindTriSYCL.cmake)

# Create targets for each example source file
set(example_files
    examples/vector_add_tmp_sycl.cpp
    examples/vector_add_tmp_sycl_scalar.cpp)

foreach(example_source IN LISTS example_files)
    get_filename_component(example_target ${example_source} NAME_WE)
    add_executable(${example_target} ${example_source})
    target_compile_options(${example_target} PRIVATE ${COINOP_DEFAULT_WARNINGS})
    add_sycl_to_target(${example_target})
endforeach()