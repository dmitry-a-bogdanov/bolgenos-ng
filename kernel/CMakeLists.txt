project(kernel ASM-ATT C)

set(cstd_flags "-std=gnu99")
set(cxxstd_flags "-std=gnu++11")

set(output_format_flags "-m32 -march=i686")
set(dep_flags "-ffreestanding")
set(warn_flags "-Wall -Wextra -Werror -Wstrict-aliasing -Winline")
set(warn_flags_cxx "-Weffc++")
set(cxx_features_flags "-fno-rtti -fno-exceptions")


set(target_hw "i686")
set(target_format "elf")
set(target_c "gcc")
set(target_cxx "g++")
set(target_c_compiler_name "${target_hw}-${target_format}-${target_c}")
set(target_cxx_compiler_name "${target_hw}-${target_format}-${target_cxx}")

find_program(found_c_compiler NAMES ${target_c_compiler_name})
find_program(found_cxx_compiler NAMES ${target_cxx_compiler_name})

if (found_c_compiler)
	set(CMAKE_C_COMPILER "${found_c_compiler}")
else()
	message(WARNING "${target_c_compiler_name} is not found!"
		" Default compiler (${CMAKE_C_COMPILER}) will be used.")
endif()

if (found_cxx_compiler)
	set(CMAKE_CXX_COMPILER "${found_cxx_compiler}")
else()
	message(WARNING "${target_cxx_compiler_name} is not found!"
		" Default compiler (${CMAKE_CXX_COMPILER}) will be used.")
endif()

message(STATUS
	"Toolchain information\n"
	"C Compiler = ${CMAKE_C_COMPILER}\n"
	"CXX Compiler = ${CMAKE_CXX_COMPILER}")


# FIXME: looks like a crap
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_C_FLAGS "${warn_flags} ${output_format_flags} ${dep_flags} ${cstd_flags}")
set(CMAKE_CXX_FLAGS "${warn_flags} ${warn_flags_cxx} ${output_format_flags} ${dep_flags} ${cxxstd_flags} ${cxx_features_flags}")
set(CMAKE_ASM-ATT_COMPILER "${CMAKE_C_COMPILER}")
set(CMAKE_ASM-ATT_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_ASM-ATT_COMPILE_OBJECT ${CMAKE_C_COMPILE_OBJECT})

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


set(m4_generated_files "")

foreach(m4_file ${m4_input_files})
	get_filename_component(in_basename ${m4_file} NAME)
	string(REGEX REPLACE ".m4" "" out_basename ${in_basename})
	set(out_file "${m4_output_dir}/${out_basename}")
	set(m4_generated_files "${out_file}" ${m4_generated_file})
	message("adding target ${out_file}")
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
	asm.cpp
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
	pic_8259.cpp
	pic_common.cpp
	pit.cpp
	printk.cpp
	ps2.cpp
	time.cpp
	traps.hpp
	traps.cpp
	vga_console.cpp
	
	keyboard.cpp
	ps2_keyboard.hpp
	ps2_keyboard.cpp
	ps2_keyboard_sm.hpp
	ps2_keyboard_sm.cpp
	
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
	
	abi/cxxabi.c
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


message("gen_files = ${m4_generated_files}")
message("config_h = ${config_h}")



message("kernel_sources = ${kernel_sources}")
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
