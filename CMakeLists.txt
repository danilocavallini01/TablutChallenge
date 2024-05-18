cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(tablutAiChallange VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(ai main.cpp)
