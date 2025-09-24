cmake_minimum_required(VERSION 3.15)
project(nstd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library (header-only)
add_library(nstd INTERFACE)
target_include_directories(nstd INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

