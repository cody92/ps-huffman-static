cmake_minimum_required(VERSION 2.8.4)
project(ps_huffman)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libgcc -static-libstdc++")

set(SOURCE_FILES main.cpp)
add_executable(ps_huffman ${SOURCE_FILES})