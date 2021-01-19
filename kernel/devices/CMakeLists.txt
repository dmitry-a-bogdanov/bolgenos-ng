project(libdevices)

add_subdirectory(ps2)
add_subdirectory(interrupt_controller)

add_library(libdevices STATIC
	src/frequency_divider.cpp
	src/frequency_divider.hpp
	src/keyboard.cpp
	src/pit.cpp
	src/vga_console.cpp
	)
target_include_directories(libdevices PUBLIC include)
target_link_libraries(libdevices PUBLIC
	libps2
	interrupt_controller
	PRIVATE
	libx86)