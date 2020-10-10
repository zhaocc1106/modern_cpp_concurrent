cmake_minimum_required(VERSION 3.15)
project(atomic)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(atomic memBarriers.cpp)

# boost begin
set(Boost_DETAILED_FAILURE_MSG ON)
set(Boost_DEBUG on)
set(BOOST_ROOT C:/boost_1_74_0)
set(BOOST_INCLUDEDIR ${BOOST_ROOT})
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
find_package(Boost COMPONENTS REQUIRED)
include_directories(${BOOST_ROOT})
target_link_libraries(atomic ${Boost_LIBRARIES})
# boost end

# 添加-fno-elide-constructors编译参数能够防止编译对构造函数进行优化
#set(CMAKE_CXX_FLAGS "-fno-elide-constructors ${CMAKE_CXX_FLAGS}")