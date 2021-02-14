cmake_minimum_required(VERSION 3.0.2)
project(testing
	CXX)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(some_test some_test.cpp)

add_subdirectory(googletest)
