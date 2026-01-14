# FetchContent needs CMake 3.14+
cmake_minimum_required(VERSION 3.14)
project(lscpp VERSION 1.0 LANGUAGES CXX)

# Enable Clangd LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG	12.1.0
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
	CLI11
	GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
	GIT_TAG v2.6.1
)
FetchContent_MakeAvailable(CLI11)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(lscpp ${SOURCES})

target_include_directories(lscpp PRIVATE include)

target_link_libraries(lscpp PRIVATE CLI11::CLI11 fmt::fmt)
