project(log)

add_library(log STATIC
	include/log.hpp
	include/loggable.hpp

	log.cpp
	loggable.cpp
	vga_buf.cpp
	vga_buf.hpp
	vga_log_buf.cpp
	vga_log_buf.hpp
	)

target_include_directories(log PUBLIC include)
target_link_libraries(log PUBLIC libkernelcxx)
