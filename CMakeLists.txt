cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(meetupcpp2019 CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/triSYCL/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON CACHE BOOL
    "Generate compile commands database; set to ON to make clang tooling happy")
set(MEETUPCPP_TRISYCL_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/external/triSYCL CACHE PATH
    "Path of the triSYCL source to be used when triSYCL is selected")
set(COMPUTECPP_BITCODE
    ptx64 CACHE STRING
    "Bitcode generation target for ComputeCpp; defaults to NVIDIA PTX")
set(TRISYCL_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/external/triSYCL/include CACHE PATH
    "triSYCL implementation include directory; defaults to vendored submodule")
set(MEETUPCPP_SYCL
    triSYCL CACHE STRING
    "SYCL implementation to be used")

# Ensure the selected SYCL implementation is supported
set(MEETUPCPP_SYCL_SUPPORTED_VALUES triSYCL ComputeCpp)
set_property(CACHE MEETUPCPP_SYCL PROPERTY STRINGS ${MEETUPCPP_SYCL_SUPPORTED_VALUES})
if(NOT MEETUPCPP_SYCL IN_LIST MEETUPCPP_SYCL_SUPPORTED_VALUES)
    message(FATAL_ERROR "MEETUPCPP_SYCL must be one of: ${MEETUPCPP_SYCL_SUPPORTED_VALUES}")
endif()
message(STATUS "SYCL implementation: ${MEETUPCPP_SYCL}")

set(MEETUPCPP_DEFAULT_WARNINGS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror -Wall -Wextra -pedantic-errors>
    $<$<CXX_COMPILER_ID:MSVC>:
        W4>
)

find_package(OpenCL)

include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
endif()

#
# Setup the selected SYCL implementation
# This will define the unary function: add_sycl_to_target(<target>)
#
if (MEETUPCPP_SYCL MATCHES "^triSYCL$")
    # If OpenCL has been found, enable it by default
    set(TRISYCL_OPENCL
        ${OpenCL_FOUND} CACHE BOOL "")
endif()

find_package(${MEETUPCPP_SYCL} REQUIRED)
if(NOT COMMAND add_sycl_to_target)
    message(FATAL_ERROR "Cannot find SYCL CMake helper function 'add_sycl_to_target()', "
                        "please check your SYCL installation.")
endif()

set(sycl_example_files
    examples/vector_add_sycl_scalar.cpp
    examples/vector_add_sycl.cpp
    examples/vector_add_tmp_sycl.cpp
    examples/vector_add_tmp_sycl_scalar.cpp
    examples/matrix_add_sycl.cpp)

set(opencl_example_files 
    examples/vector_add_opencl.cpp)

set(cuda_example_files
    examples/vector_add_cuda.cu)

#
# Create targets for each example source file
#
foreach(example_source IN LISTS sycl_example_files)
    get_filename_component(example_target ${example_source} NAME_WE)
    add_executable(${example_target} ${example_source})
    target_compile_options(${example_target} PRIVATE ${COINOP_DEFAULT_WARNINGS})
    # TODO wrap different add_sycl_to_target APIs with an adapter function
    if (MEETUPCPP_SYCL MATCHES "^ComputeCpp$")
        # ComputeCpp helper needs named parameters
        add_sycl_to_target(TARGET ${example_target} SOURCES ${example_source})
    else()
        add_sycl_to_target(${example_target})
    endif()
endforeach()

if(OpenCL_FOUND)
    foreach(example_source IN LISTS opencl_example_files)
        get_filename_component(example_target ${example_source} NAME_WE)
        add_executable(${example_target} ${example_source})
        target_compile_options(${example_target} PRIVATE ${COINOP_DEFAULT_WARNINGS})
        target_link_libraries(${example_target} PRIVATE OpenCL::OpenCL)
    endforeach()
endif()

if(CMAKE_CUDA_COMPILER)
    foreach(example_source IN LISTS cuda_example_files)
        get_filename_component(example_target ${example_source} NAME_WE)
        add_executable(${example_target} ${example_source})
        target_compile_options(${example_target} PRIVATE ${COINOP_DEFAULT_WARNINGS})
    endforeach()
endif()
