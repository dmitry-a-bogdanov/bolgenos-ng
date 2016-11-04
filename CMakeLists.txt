cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0048 NEW)
project(bolgenos-ng
	VERSION 0.0.4
	LANGUAGES NONE)

set(kernel_headers_dir "include/")
include_directories(${kernel_headers_dir})
file(GLOB_RECURSE kernel_headers include/*.h include/*.hpp)

set(kernel_config_dir "config/")
add_subdirectory(${kernel_config_dir})

add_subdirectory(kernel)

# Doxygen project
add_subdirectory(doc)

