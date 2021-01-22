project(log)

add_library(log STATIC
	include/bolgenos-ng/loggable.hpp
	include/bolgenos-ng/log.hpp
	include/loggable.hpp
	include/log.hpp
	include/log/streambufs.hpp

	src/base_log_buf.hpp
	src/buf_builder.cpp
	src/buf_builder.hpp
	src/delegating_log_buf.hpp
	src/log.cpp
	src/log_buf.cpp
	src/log_buf.hpp
	src/loggable.cpp
	src/serial_buf.cpp
	src/serial_buf.hpp
	src/simple_stream_buf.cpp
	src/simple_stream_buf.hpp
	src/streambufs.cpp
	src/vga_buf.cpp
	src/vga_buf.hpp
	src/vga_log_buf.cpp
	src/vga_log_buf.hpp
)

target_include_directories(log PUBLIC include)
target_link_libraries(log PUBLIC libkernelcxx)
target_link_libraries(log PRIVATE libdevices)
