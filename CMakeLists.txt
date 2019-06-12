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
        ${OpenCL_FOUND} CACHE BOOL "Enable OpenCL backend for triSYCL")
    # Download via submodule
    execute_process(COMMAND git submodule update --init -- external/triSYCL
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

find_package(${MEETUPCPP_SYCL} REQUIRED)
if(NOT COMMAND add_sycl_to_target)
    message(FATAL_ERROR
        "Cannot find SYCL CMake helper function `add_sycl_to_target()`, "
        "please check your SYCL installation.")
endif()

set(sycl_example_files
    examples/vector_add_sycl_serial.cpp
    examples/vector_add_sycl.cpp
    examples/vector_add_tmp_sycl.cpp
    examples/vector_add_tmp_sycl_serial.cpp
    examples/matrix_add_sycl.cpp)

set(opencl_example_files 
    examples/vector_add_opencl.cpp)

set(cuda_example_files
    examples/vector_add_cuda.cu)

#
# Create targets for each example source file
#
function(add_example IN_SOURCE OUT_TARGET)
    get_filename_component(tgt ${IN_SOURCE} NAME_WE)
    add_executable(${tgt} ${IN_SOURCE})
    get_source_file_property(src_language ${IN_SOURCE} LANGUAGE)
    # CMake <= 3.10 doesn't support $<$<COMPILE_LANGUAGE:CUDA> at all,
    # we need to split target_compile_options by hand without relying
    # on generator expressions
    if(src_language STREQUAL "CUDA")
        # nvcc is stuck to C++14, thanks nvidia
        target_compile_features(${tgt} PRIVATE cxx_std_14)
        target_compile_options(${tgt}
            PUBLIC
                $<$<CXX_COMPILER_ID:MSVC>:--compiler-options /W4>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:--compiler-options -Wall,-Wextra>)
    else()
        target_compile_features(${tgt} PRIVATE cxx_std_17)
        target_compile_options(${tgt}
            PUBLIC
                $<$<CXX_COMPILER_ID:MSVC>:/W4>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>)
    endif()
    set_target_properties(${tgt} PROPERTIES CXX_EXTENSIONS OFF)
    add_test(NAME "test_${tgt}" COMMAND ${tgt} 100)
    set(${OUT_TARGET} ${tgt} PARENT_SCOPE)
endfunction()

enable_testing()

foreach(example_source IN LISTS sycl_example_files)
    add_example(${example_source} example_target)
    # TODO wrap different add_sycl_to_target APIs with an adapter function
    if (MEETUPCPP_SYCL MATCHES "^ComputeCpp$")
        # ComputeCpp helper needs sources passed in as arguments
        add_sycl_to_target(TARGET ${example_target} SOURCES ${example_source})
    else()
        add_sycl_to_target(${example_target})
    endif()
endforeach()

if(OpenCL_FOUND)
    foreach(example_source IN LISTS opencl_example_files)
        add_example(${example_source} example_target)
        target_link_libraries(${example_target} PRIVATE OpenCL::OpenCL)
    endforeach()
endif()

if(CMAKE_CUDA_COMPILER)
    foreach(example_source IN LISTS cuda_example_files)
        add_example(${example_source} example_target)
    endforeach()
endif()
