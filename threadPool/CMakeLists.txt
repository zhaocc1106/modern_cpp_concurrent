cmake_minimum_required(VERSION 3.17)
project(threadPool)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(threadPool
        include/thread_safe_queue.hpp
        src/test.cpp
        include/threads_joiner.hpp
        include/simple_thread_pool.hpp
        include/futured_thread_pool.hpp)
