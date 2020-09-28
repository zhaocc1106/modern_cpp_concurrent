cmake_minimum_required(VERSION 3.15)
project(concurrent_learn)

set(CMAKE_CXX_STANDARD 14)


add_executable(concurrent_learn sharedDataBetweenThreads/sharedMutex.cpp)

# boost begin
set(Boost_DETAILED_FAILURE_MSG ON)
set(Boost_DEBUG on)
set(BOOST_ROOT C:/boost_1_74_0)
set(BOOST_INCLUDEDIR ${BOOST_ROOT})
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
find_package(Boost COMPONENTS REQUIRED)
include_directories(${BOOST_ROOT})
target_link_libraries(concurrent_learn ${Boost_LIBRARIES})
# boost end