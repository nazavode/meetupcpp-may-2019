cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(meetupcpp2019 LANGUAGES CXX)

# Always enable generation of compilation database to
# make clang tooling and vscode happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generate compile commands database" FORCE)

set(MEETUPCPP_DEFAULT_WARNINGS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror -Wall -Wextra -pedantic-errors>
    $<$<CXX_COMPILER_ID:MSVC>:
        W4>
)

include(GNUInstallDirs)
