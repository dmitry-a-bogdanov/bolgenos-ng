project(bolgenos-ng)
cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/;"
	${CMAKE_MODULE_PATH})

add_subdirectory(boot)
add_subdirectory(kernel)
add_subdirectory(tools)

