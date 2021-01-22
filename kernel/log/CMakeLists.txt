project(log)

add_library(log STATIC
	include/loggable.hpp
	include/log_level.hpp
	include/log.hpp

	src/buf_builder.cpp
	src/buf_builder.hpp
	src/delegating_log_buf.hpp
	src/log.cpp
	src/loggable.cpp
	src/serial_buf.cpp
	src/serial_buf.hpp
	src/simple_stream_buf.cpp
	src/simple_stream_buf.hpp
	src/streambufs.cpp
	src/streambufs.hpp
	src/vga_buf.cpp
	src/vga_buf.hpp
	src/vga_log_buf.cpp
	src/vga_log_buf.hpp
)

target_include_directories(log PUBLIC include)
target_link_libraries(log PUBLIC libkernelcxx)
target_link_libraries(log PRIVATE libdevices)
