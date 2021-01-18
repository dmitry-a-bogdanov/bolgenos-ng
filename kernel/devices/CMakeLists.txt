project(libdevices)

add_subdirectory(ps2)
add_subdirectory(interrupt_controller)

add_library(libdevices STATIC libdevices.cpp)
target_link_libraries(libdevices PUBLIC
	libps2
	interrupt_controller)