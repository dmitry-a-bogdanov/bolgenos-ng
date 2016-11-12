project(testing
	CXX)

add_definitions(-std=c++11)

add_executable(test_generic_list test_generic_list.cpp)
target_link_libraries(test_generic_list gtest_main)
