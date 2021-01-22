project(serial)

add_library(serial STATIC
	include/serial/com_ports.hpp
	include/serial/io_ports.hpp
	include/serial/lcr.hpp
	include/serial/lsr.hpp
	include/serial/serial_port.hpp

	src/io_ports.cpp
	src/lcr.cpp
	src/lsr.cpp
	src/serial_port.cpp
)

target_link_libraries(serial PRIVATE libdevices)
target_include_directories(serial
	PUBLIC include
	PRIVATE include/serial)