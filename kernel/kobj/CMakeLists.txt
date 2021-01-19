project(kobj)

add_library(kobj STATIC
	include/bolgenos-ng/kernel_object.hpp
	include/bolgenos-ng/multiboot_info.hpp

	src/kernel_object.cpp
	src/multiboot_header.cpp
	src/multiboot_info.cpp
	)

target_include_directories(kobj PUBLIC include/)
target_link_libraries(kobj PRIVATE libkernelcxx)
target_link_options(kobj INTERFACE -Wl,--undefined=multiboot_header)