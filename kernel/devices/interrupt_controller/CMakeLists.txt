project(interrupt_controller)

add_library(interrupt_controller STATIC
	include/bolgenos-ng/interrupt_controller.hpp

	interrupt_controller.cpp
	pic_8259.cpp
	pic_8259.hpp
	)

target_include_directories(interrupt_controller PUBLIC include)
target_link_libraries(interrupt_controller PRIVATE
	libkernelcxx libx86)