cmake_minimum_required(VERSION 3.15)
project(concurrent_learn)

set(CMAKE_CXX_STANDARD 14)

add_executable(concurrent_learn atomic/memBarriers.cpp)