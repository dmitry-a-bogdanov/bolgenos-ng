cmake_minimum_required(VERSION 3.13.0)
cmake_policy(SET CMP0048 NEW)
project(bolgenos-ng
	VERSION 0.0.4
	LANGUAGES ASM-ATT C CXX)

if (NOT ${CMAKE_CROSSCOMPILING})
	message(FATAL_ERROR "cmake must be configured for cross-compiling")
endif()

add_subdirectory(config)

message(STATUS "Compiling BOLGENOS-NG using toolchain: "
	"${CMAKE_TOOLCHAIN_FILE}")

set(target_binary_format "Generic")
set(target_processor "i686")

if (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "${target_binary_format}"))
	message(FATAL_ERROR "BOLGENOS-NG can be compiled only in "
		" '${target_binary_format}'"
		" but '${CMAKE_SYSTEM_NAME}' is supplied")
endif ()


if (NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "${target_processor}")
	message(FATAL_ERROR "BOLGENOS-NG supports only ${target_processor} "
		"but ${CMAKE_SYSTEM_PROCESSOR} is supplied")
endif ()


set(cstd_flags "-std=gnu99")
set(cxxstd_flags "-std=gnu++20 -fno-exceptions -ftree-ter")

set(opt_flags "-O3")

#set(warn_flags "-Wall -Wextra -Werror -Wstrict-aliasing -Winline")
set(warn_flags "-Wall -Wextra -Werror -Wstrict-aliasing")
set(warn_flags_cxx "-Weffc++")


unset(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
unset(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
unset(CMAKE_EXE_LINKER_FLAGS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warn_flags} ${cstd_flags} ${opt_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warn_flags} ${warn_flags_cxx} ${cxxstd_flags} ${opt_flags}")
set(CMAKE_ASM-ATT_COMPILE_OBJECT ${CMAKE_C_COMPILE_OBJECT})

message(STATUS "Toolchain information:")
foreach (compiler CMAKE_C_COMPILER CMAKE_C_COMPILER CMAKE_ASM-ATT_COMPILER)
	message(STATUS "${compiler} = ${${compiler}}")
endforeach ()

message(STATUS "Compiler is configured. Adding targets..")

set(kernel_binary "kernel.bin")

include_directories(${CONFIG_INCLUDE_DIRS})

# Variable $kernel_start_file should keep source file that will be included
# in kernel of any configuration and can be compiled very fast.
# This file is used for forcing relinking of resulting object.
set(kernel_start_file "${CMAKE_CURRENT_SOURCE_DIR}/start.sx")

set(kernel_linker "${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld")

set_source_files_properties(${kernel_start_file} PROPERTIES
	LANGUAGE "ASM-ATT")


message(STATUS "Build configuration: ${config_h}")

message("C = ${CMAKE_C_FLAGS}, CXX = ${CMAKE_CXX_FLAGS}")
add_executable(${kernel_binary}
	${kernel_start_file}
	${config_h}

	main.cpp
)

add_subdirectory(libkernelcxx)
add_subdirectory(memory)
add_subdirectory(log)
add_subdirectory(x86)
add_subdirectory(threading)
add_subdirectory(sched)
add_subdirectory(devices)
add_subdirectory(kobj)
add_subdirectory(ost)

# custom target is always out-of-date
add_custom_target(__relink_kernel
	COMMAND touch ${kernel_start_file}
	DEPENDS ${kernel_linker}
	COMMENT "Forcing kernel relinking..")

add_dependencies(${kernel_binary} __relink_kernel)

target_link_libraries(${kernel_binary}
	PRIVATE
	libkernelcxx
	memory
	libthreading
	libx86
	libdevices
	sched
	log
	ost
	PUBLIC
	kobj
	)

set_target_properties(${kernel_binary} PROPERTIES
	LINK_FLAGS " -T ${kernel_linker} -nostdlib")

set(qemu_debug_options -d guest_errors,mmu,pcall,int,unimp)
set(qemu_command qemu-system-i386 -kernel ${kernel_binary})

add_custom_target(start_qemu
	COMMENT "Starting kernel in Qemu..."
	COMMAND ${qemu_command}
	DEPENDS ${kernel_binary})

add_custom_target(start_qemu_debug
	COMMENT "Starting kernel in Qemu..."
	COMMAND ${qemu_command} ${qemu_debug_options}
	DEPENDS ${kernel_binary})
