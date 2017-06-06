cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0048 NEW)
project(bolgenos-ng
	VERSION 0.0.4
	LANGUAGES NONE)

set(kernel_headers_dir "include/")
include_directories(${kernel_headers_dir})
file(GLOB_RECURSE kernel_headers include/*.h include/*.hpp)

if (${CMAKE_CROSSCOMPILING})
	message(STATUS "CMake is being run in crosscompiling mode. "
		"Kernel targets will be built.")
	set(kernel_config_dir "config/")
	add_subdirectory(${kernel_config_dir})

	add_subdirectory(kernel)

# Doxygen project
	add_subdirectory(doc)
else()
	message(STATUS "CMake is being run NOT in crosscompiling mode. "
		" Only testing project will be built.")
	enable_testing()

	# Testing project. It should not use toolchain of kernel.
	add_subdirectory(testing)
endif (${CMAKE_CROSSCOMPILING})
