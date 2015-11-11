project(bolgneos-ng ASM-ATT C)

# FIXME: will not work on different machines
set(CMAKE_CXX_COMPILER i686-elf-g++)
set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_ASM-ATT_COMPILER i686-elf-gcc)

# FIXME: looks like a crap
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_C_FLAGS "-ffreestanding -Wall -Wextra -Werror")
set(CMAKE_ASM-ATT_COMPILER "${CMAKE_C_COMPILER}")
set(CMAKE_ASM-ATT_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_ASM-ATT_COMPILE_OBJECT ${CMAKE_C_COMPILE_OBJECT})

set(kernel_binary "kernel.bin")

# Variable $kernel_start_file should keep source file that will be included
# in kernel of any configuration and can be compiled very fast.
# This file can be used for forcing relinking and other dirty hacks.
set(kernel_start_file "${CMAKE_CURRENT_SOURCE_DIR}/start.sx")

set(kernel_sources ${kernel_start_file}
	main.c
	bootstrap.h
	vga_console.h
	vga_console.c)

set(kernel_linker "${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld")

set_source_files_properties(${kernel_start_file} PROPERTIES
	LANGUAGE "ASM-ATT")

add_executable(${kernel_binary} ${kernel_sources} ${kernel_headers})

# custom target is always out-of-date
add_custom_target(__relink_kernel
	COMMAND touch ${kernel_start_file}
	DEPENDS ${kernel_linker}
	COMMENT "Forcing kernel relinking..")

add_dependencies(${kernel_binary} __relink_kernel)

set_target_properties(${kernel_binary} PROPERTIES
	LINK_FLAGS "-T ${kernel_linker} -nostdlib")

add_custom_target(start_qemu
	COMMENT "Starting kernel in Qemu..."
	COMMAND qemu-system-i386 -kernel ${kernel_binary})
add_dependencies(start_qemu ${kernel_binary})
