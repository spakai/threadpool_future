project(indexes)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_VERBOSE_MAKEFILE on)
include_directories($ENV{GMOCK_HOME}/include $ENV{GMOCK_HOME}/gtest/include)
link_directories($ENV{GMOCK_HOME}/lib $ENV{GMOCK_HOME}/gtest/lib)

add_definitions(-std=c++1y)

set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -Wall -g -O0 --coverage")

set(sources 
   main.cpp
   ThreadPool.h
   TestTimer.cpp
   )

set(test_sources
   ThreadPoolTest.cpp)

add_executable(test ${test_sources} ${sources})

target_link_libraries(test pthread)
target_link_libraries(test gmock)
target_link_libraries(test gtest)
