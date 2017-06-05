project(kernel
	LANGUAGES ASM-ATT C CXX)

message(STATUS "Compiling BOLGENOS-NG using toolchain: "
	"${CMAKE_TOOLCHAIN_FILE}")

set(target_binary_format "Generic")
set(target_processor "i686")

if (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "${target_binary_format}"))
	message(FATAL_ERROR "BOLGENOS-NG can be compiled only in "
		" '${target_binary_format}'"
		" but '${CMAKE_SYSTEM_NAME}' is supplied")
endif()


if (NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "${target_processor}")
	message(FATAL_ERROR "BOLGENOS-NG supports only ${target_processor} "
		"but ${CMAKE_SYSTEM_PROCESSOR} is supplied")
endif()


set(cstd_flags "-std=gnu99 -fno-exceptions")
set(cxxstd_flags "-std=gnu++11 -fno-exceptions")

set(opt_flags "-O3")

set(warn_flags "-Wall -Wextra -Werror -Wstrict-aliasing -Winline")
set(warn_flags_cxx "-Weffc++")



unset(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
unset(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
unset(CMAKE_EXE_LINKER_FLAGS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warn_flags} ${cstd_flags} ${opt_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warn_flags} ${warn_flags_cxx} ${cxxstd_flags} ${opt_flags}")
set(CMAKE_ASM-ATT_COMPILE_OBJECT ${CMAKE_C_COMPILE_OBJECT})

message(STATUS "Toolchain information:")
foreach(compiler CMAKE_C_COMPILER CMAKE_C_COMPILER CMAKE_ASM-ATT_COMPILER)
	message(STATUS "${compiler} = ${${compiler}}")
endforeach()

message(STATUS "Compiler is configured. Adding targets..")

set(kernel_binary "kernel.bin")

include_directories(${CONFIG_INCLUDE_DIRS})

# Variable $kernel_start_file should keep source file that will be included
# in kernel of any configuration and can be compiled very fast.
# This file is used for forcing relinking of resulting object.
set(kernel_start_file "${CMAKE_CURRENT_SOURCE_DIR}/start.sx")

set(m4_input_dir "${CMAKE_CURRENT_SOURCE_DIR}/m4")
file(GLOB_RECURSE m4_input_files "${m4_input_dir}/*.cpp.m4")

set(m4_output_dir "${CMAKE_CURRENT_BINARY_DIR}/m4_generated")
file(MAKE_DIRECTORY ${m4_output_dir})


foreach(m4_file ${m4_input_files})
	get_filename_component(in_basename ${m4_file} NAME)
	string(REGEX REPLACE ".m4" "" out_basename ${in_basename})
	set(out_file "${m4_output_dir}/${out_basename}")
	set(m4_generated_files "${out_file}" ${m4_generated_file})
	message(STATUS "Found m4-generated file \"${out_file}\"")
	add_custom_command(OUTPUT "${out_file}"
		COMMAND m4 -P "${m4_file}" > "${out_file}"
		COMMENT m4 -P "${m4_file}" > "${out_file}"
		WORKING_DIRECTORY "${m4_input_dir}"
		DEPENDS "${m4_file}")
endforeach(m4_file)

set(kernel_sources ${kernel_start_file}
	${config_h}
	${ost_h}

	${m4_generated_files}
	error.cpp
	frequency_divider.hpp
	frequency_divider.cpp
	irq.cpp
	kernel_object.cpp
	main.cpp
	memory.cpp
	mmu.cpp
	multiboot_header.c
	multiboot_info.cpp
	pit.cpp
	printk.cpp
	time.cpp
	traps.hpp
	traps.cpp
	vga_console.cpp
	
	devices/interrupt_controller/interrupt_controller.cpp
	devices/interrupt_controller/pic_8259.hpp
	devices/interrupt_controller/pic_8259.cpp
	
	ps2/common.cpp
	ps2/ps2_controller.cpp
	ps2/ps2_line.cpp
	keyboard.cpp
	ps2/ps2_keyboard.hpp
	ps2/ps2_keyboard.cpp
	ps2/ps2_keyboard_sm.hpp
	ps2/ps2_keyboard_sm.cpp
	
	buddy_allocator.hpp
	buddy_allocator.cpp
	free_list.hpp
	free_list.cpp
	mallocator.hpp
	mallocator.cpp
	page_allocator.hpp
	page_allocator.cpp
	page.cpp
	slab.cpp
	
	abi/cxxabi.cpp
	backtrace.cpp
	
	lib/snprintf.cpp
	lib/streambuf.hpp
	lib/streambuf.cpp
	lib/string.cpp
	lib/ostream.cpp
	lib/vga_buf.hpp
	lib/vga_buf.cpp
	lib/vga_log_buf.hpp
	lib/vga_log_buf.cpp
	
	ost/ost.cpp
	ost/bitarray.hpp
	ost/bitarray.cpp
	ost/memory.hpp
	ost/memory.cpp
	)

set_source_files_properties(${m4_generated_files}
	PROPERTIES GENERATED TRUE)

set(kernel_linker "${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld")

set_source_files_properties(${kernel_start_file} PROPERTIES
	LANGUAGE "ASM-ATT")


message(STATUS "Build configuration: ${config_h}")



message(STATUS "List of kernel source files: ${kernel_sources}")
add_executable(${kernel_binary}
	${kernel_headers}
	${kernel_sources}
)

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
