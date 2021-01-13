add_subdirectory(ps2)

add_library(libdevices STATIC libdevices.cpp)
target_link_libraries(libdevices PUBLIC
	libps2)