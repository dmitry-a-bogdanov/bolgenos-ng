cmake_policy(SET CMP0048 NEW)
project(bolgenos-ng
	VERSION 0.0.2)
cmake_minimum_required(VERSION 3.0.2)

add_definitions(-DBOLGENOS_NG_VERSION="${PROJECT_VERSION}")

set(kernel_headers_dir "include/")
include_directories(${kernel_headers_dir})
file(GLOB_RECURSE kernel_headers include/*.h)

add_subdirectory(kernel)

# Doxygen project
add_subdirectory(doc)

