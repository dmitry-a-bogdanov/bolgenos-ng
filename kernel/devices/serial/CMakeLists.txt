project(serial)

add_library(serial STATIC
	serial_port.cpp
)

target_link_libraries(serial PRIVATE libdevices)
target_include_directories(serial
	PUBLIC include
	PRIVATE include/serial)