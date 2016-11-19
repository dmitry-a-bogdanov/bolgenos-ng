project(testing
	CXX)

add_definitions(-std=c++11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

function(create_test name)
	add_executable(${name} tests/${name}.cpp)
	target_link_libraries(${name} gtest_main)
	add_test(NAME ${name}
		COMMAND ${name})
endfunction(create_test)

create_test(basic_forward_list)
create_test(forward_list)
create_test(free_list)
create_test(forward_list_iterators)

