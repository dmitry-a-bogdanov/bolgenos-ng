project(testing
	CXX)


message(STATUS "Toolchain information:")
foreach(compiler CMAKE_C_COMPILER CMAKE_C_COMPILER CMAKE_ASM-ATT_COMPILER)
	message(STATUS "${compiler} = ${${compiler}}")
endforeach()

add_executable(testing testing_main.cpp)
