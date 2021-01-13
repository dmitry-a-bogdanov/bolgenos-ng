project(libps2)

add_library(libps2 STATIC
	common.cpp
	controller.cpp
	line.cpp

	ps2_keyboard.hpp
	ps2_keyboard.cpp
	ps2_keyboard_sm.hpp
	ps2_keyboard_sm.cpp
)

target_include_directories(libps2 PUBLIC ../include)
target_link_libraries(libps2 PUBLIC libkernelcxx)