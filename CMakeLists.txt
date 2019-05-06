cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(meetupcpp2019 CXX)

# Always enable generation of compilation database to
# make clang tooling and vscode happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generate compile commands database" FORCE)

set(MEETUPCPP_SYCL_SUPPORTED_VALUES triSYCL ComputeCpp)
set(MEETUPCPP_SYCL triSYCL CACHE STRING "SYCL implementation to be used")
set_property(CACHE MEETUPCPP_SYCL PROPERTY STRINGS ${MEETUPCPP_SYCL_SUPPORTED_VALUES})
if(NOT MEETUPCPP_SYCL IN_LIST MEETUPCPP_SYCL_SUPPORTED_VALUES)
    message(FATAL_ERROR "MEETUPCPP_SYCL must be one of ${MEETUPCPP_SYCL_SUPPORTED_VALUES}")
endif()
message(STATUS "SYCL implementation is: ${MEETUPCPP_SYCL}")

set(MEETUPCPP_DEFAULT_WARNINGS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror -Wall -Wextra -pedantic-errors>
    $<$<CXX_COMPILER_ID:MSVC>:
        W4>
)

#
# Setup the selected SYCL implementation
# This will define the unary function: add_sycl_to_target(<target>)
#

if (MEETUPCPP_SYCL MATCHES "^triSYCL$")
    # Enable vendored triSYCL
    set(TRISYCL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples/external/triSYCL/include CACHE PATH "" FORCE)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/examples/external/triSYCL/cmake)
    find_package(triSYCL REQUIRED)
endif()

if (MEETUPCPP_SYCL MATCHES "^ComputeCpp$")
    set(COMPUTECPP_BITCODE ptx64 CACHE STRING "" FORCE)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    find_package(ComputeCpp REQUIRED)
endif()

# Create targets for each example source file
set(example_files
    examples/vector_add_sycl_scalar.cpp
    examples/vector_add_sycl.cpp
    examples/vector_add_tmp_sycl.cpp
    examples/vector_add_tmp_sycl_scalar.cpp)

foreach(example_source IN LISTS example_files)
    get_filename_component(example_target ${example_source} NAME_WE)
    add_executable(${example_target} ${example_source})
    # target_compile_options(${example_target} PRIVATE ${COINOP_DEFAULT_WARNINGS})
    add_sycl_to_target(TARGET ${example_target} SOURCES ${example_source})
endforeach()
