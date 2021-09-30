cmake_minimum_required(VERSION 3.16.3)
project(threadPool)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

# boost begin
set(Boost_DETAILED_FAILURE_MSG ON)
set(Boost_DEBUG on)
set(BOOST_ROOT /home/zhaocc/env/boost_1_68_0)
set(BOOST_INCLUDEDIR ${BOOST_ROOT}/)
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
find_package(Boost COMPONENTS REQUIRED thread)
include_directories(${BOOST_INCLUDEDIR})
# boost end

add_executable(threadPool
        #        include/thread_safe_queue.hpp
        #        src/my_thread_pool_test.cpp
        #        include/threads_joiner.hpp
        #        include/simple_thread_pool.hpp
        #        include/futured_thread_pool.hpp
        #        include/parallel_quick_sort.hpp
        #        include/multi_queue_thread_pool.hpp
        #        src/boost_thread_pool_test.cpp
        include/thread_pool_timer_container.h
        src/thread_pool_timer_container.cpp
        src/thread_pool_timer_test.cpp)

target_link_libraries(threadPool ${Boost_LIBRARIES})
