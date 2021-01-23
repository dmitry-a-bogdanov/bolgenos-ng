project(log)

add_library(log STATIC
	include/log/composite_buf.hpp
	include/log/delegating_log_buf.hpp
	include/loggable.hpp
	include/logger.hpp
	include/log.hpp
	include/log_level.hpp
	include/log/serial_buf.hpp
	include/log/simple_stream_buf.hpp
	include/log/vga_log_buf.hpp

	src/buf_builder.cpp
	src/buf_builder.hpp
	src/delegating_log_buf.cpp
	src/log.cpp
	src/loggable.cpp
	src/logger.cpp
	src/serial_buf.cpp
	src/simple_stream_buf.cpp
	src/streambufs.cpp
	src/streambufs.hpp
	src/vga_buf.cpp
	src/vga_buf.hpp
	src/vga_log_buf.cpp

)

target_include_directories(log PUBLIC include)
target_link_libraries(log PUBLIC libkernelcxx)
target_link_libraries(log PRIVATE libdevices)
