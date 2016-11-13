project(testing
	CXX)

add_definitions(-std=c++11)

function(create_test name)
	add_executable(${name} ${name}.cpp)
	target_link_libraries(${name} gtest_main)
	add_test(NAME ${name}
		COMMAND ${name})
endfunction(create_test)

create_test(test_forward_list)
create_test(test_forward_list_iterators)
create_test(test_basic_forward_list)

