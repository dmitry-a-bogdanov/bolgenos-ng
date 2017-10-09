project(kernel LANGUAGES ASM-ATT C CXX)


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


unset(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
unset(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
unset(CMAKE_EXE_LINKER_FLAGS)
unset(CMAKE_C_FLAGS)
unset(CMAKE_CXX_FLAGS)


set(CMAKE_ASM-ATT_COMPILE_OBJECT ${CMAKE_C_COMPILE_OBJECT})


message(STATUS "Toolchain information:")
foreach(compiler CMAKE_C_COMPILER CMAKE_C_COMPILER CMAKE_ASM-ATT_COMPILER)
	message(STATUS "${compiler} = ${${compiler}}")
endforeach()


set(kernel_binary_name "kernel")

set_property(SOURCE start.sx PROPERTY LANGUAGE "ASM-ATT")

set(m4_input_files
	m4/idt.cpp.m4
	m4/utils.m4
)
source_group("M4 templates" FILES ${m4_input_files})

set(m4_idt "${CMAKE_CURRENT_SOURCE_DIR}/m4/idt.cpp.m4")
set(idt "${CMAKE_CURRENT_BINARY_DIR}/idt.cpp")
add_custom_command(OUTPUT idt.cpp
	COMMAND m4 -P ${m4_idt} > ${idt}
	MAIN_DEPENDENCY ${m4_idt}
	DEPENDS m4/utils.m4
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/m4
)
set_source_files_properties(idt.cpp PROPERTIES GENERATED TRUE)


set(bolgenos_HEADERS
../include/algorithm
../include/atomic
../include/basalt/format_guard.hpp
../include/basalt/intrusive_list.hpp
../include/basalt/type_traits.hpp
../include/bolgenos-ng/asm.hpp
../include/bolgenos-ng/bitarray.hpp
../include/bolgenos-ng/compiler.h
../include/bolgenos-ng/cxxabi.h
../include/bolgenos-ng/device.h
../include/bolgenos-ng/error.h
../include/bolgenos-ng/execinfo.hpp
../include/bolgenos-ng/init_queue.hpp
../include/bolgenos-ng/interrupt_controller.hpp
../include/bolgenos-ng/io/vga/streambuf.hpp
../include/bolgenos-ng/io/vga/text_console.hpp
../include/bolgenos-ng/io/vga/types.hpp
../include/bolgenos-ng/io/vga/video_memory.hpp
../include/bolgenos-ng/irq.hpp
../include/bolgenos-ng/kernel_object.hpp
../include/bolgenos-ng/keyboard.h
../include/bolgenos-ng/log.hpp
../include/bolgenos-ng/memory.hpp
../include/bolgenos-ng/memory_region.hpp
../include/bolgenos-ng/mem_utils.hpp
../include/bolgenos-ng/mmu.hpp
../include/bolgenos-ng/multiboot_info.hpp
../include/bolgenos-ng/ost.hpp
../include/bolgenos-ng/page.hpp
../include/bolgenos-ng/pit.hpp
../include/bolgenos-ng/printk.h
../include/bolgenos-ng/ps2/common.hpp
../include/bolgenos-ng/ps2_controller.hpp
../include/bolgenos-ng/ps2/device.hpp
../include/bolgenos-ng/ps2_line.hpp
../include/bolgenos-ng/slab.hpp
../include/bolgenos-ng/string.h
../include/bolgenos-ng/time.hpp
../include/bolgenos-ng/vga_streambuf.hpp
../include/cstddef
../include/cstdint
../include/forward_list
../include/functional
../include/impl/cstddef/byte.hpp
../include/impl/functional/function.hpp
../include/impl/ios/basic_ios.hpp
../include/impl/ios/basic_ostream.hpp
../include/impl/ios/char_traits.hpp
../include/impl/ios/ios_base.hpp
../include/impl/ios/types.hpp
../include/impl/memory/default_allocator.hpp
../include/impl/memory/default_delete.hpp
../include/impl/memory/pointer_control_block.hpp
../include/impl/memory/shared_ptr.hpp
../include/impl/memory/unique_ptr.hpp
../include/impl/memory/weak_ptr.hpp
../include/impl/type_traits/enable_if.hpp
../include/impl/type_traits/integral_constant.hpp
../include/impl/type_traits/is_signed.hpp
../include/impl/type_traits/is_unsigned.hpp
../include/impl/type_traits/make_signed.hpp
../include/impl/type_traits/make_unsigned.hpp
../include/impl/type_traits/remove_cv.hpp
../include/impl/type_traits/underlying_type.hpp
../include/ios
../include/lib/free_list.hpp
../include/lib/impl/basic_forward_list.hpp
../include/m4/idt.hpp
../include/memory
../include/new
../include/ostream
../include/streambuf
../include/string
../include/type_traits
../include/utility
)
source_group("include" FILES ${bolgenos_HEADERS})


set(libkernelcxx_SOURCES
	libkernelcxx/ios_base.cpp
	libkernelcxx/new.cpp
	libkernelcxx/ostream.cpp
)
source_group("libkernelcxx" FILES ${libkernelcxx_SOURCES})


#file(GLOB_RECURSE alloc_SOURCES alloc/*)
#source_group("Allocators" FILES ${alloc_SOURCES})


set(ps2_SOURCES
	ps2/common.cpp
	ps2/ps2_controller.cpp
	ps2/ps2_keyboard.cpp
	ps2/ps2_keyboard.hpp
	ps2/ps2_keyboard_sm.cpp
	ps2/ps2_keyboard_sm.hpp
	ps2/ps2_line.cpp
)
source_group("ps2" FILES ${ps2_SOURCES})


set(basalt_SOURCES
	basalt/scoped_format_guard.cpp
)
source_group("basalt" FILES ${basalt_SOURCES})


set(lib_SOURCES
	lib/snprintf.cpp
	lib/streambuf.cpp
	lib/streambuf.hpp
	lib/string.cpp
	lib/vga_buf.cpp
	lib/vga_buf.hpp
	lib/vga_log_buf.cpp
	lib/vga_log_buf.hpp
)
source_group("lib" FILES ${lib_SOURCES})


set(ost_SOURCES
	ost/bitarray.cpp
	ost/bitarray.hpp
	ost/memory.cpp
	ost/memory.hpp
	ost/ost.cpp
	ost/type_traits.cpp
)
source_group("ost" FILES ${ost_SOURCES})


set(abi_SOURCES
	abi/cxxabi.cpp
)
source_group("abi" FILES ${abi_SOURCES})


set(dev_pic_SOURCES
	devices/interrupt_controller/interrupt_controller.cpp
	devices/interrupt_controller/pic_8259.cpp
	devices/interrupt_controller/pic_8259.hpp
)
source_group("devices/PICs" FILES ${dev_pic_SOURCES})


set(dev_vga_SOURCES
	devices/vga/text_console.cpp
)
source_group("devices/vga" FILES ${dev_vga_SOURCES})


set(kernel_SOURCES
	backtrace.cpp
	buddy_allocator.cpp
	buddy_allocator.hpp
	CMakeLists.txt
	error.cpp
	free_list.cpp
	free_list.hpp
	frequency_divider.cpp
	frequency_divider.hpp
	init_queue.cpp
	irq.cpp
	kernel_object.cpp
	keyboard.cpp
	main.cpp
	mallocator.cpp
	mallocator.hpp
	memory.cpp
	mmu.cpp
	multiboot_header.c
	multiboot_info.cpp
	page_allocator.cpp
	page_allocator.hpp
	page.cpp
	pit.cpp
	printk.cpp
	slab.cpp
	start.sx
	time.cpp
	traps.cpp
	traps.hpp
)
source_group("kernel" FILES ${kernel_SOURCES})


set(kernel_linker "${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld")


message(STATUS "Build configuration: ${config_h}")


add_executable(${kernel_binary_name}
	${config_h}
	${ost_h}

	${m4_generated_files}
	${idt}

	${alloc_SOURCES}
	${lib_SOURCES}
	${libkernelcxx_SOURCES}
	${ps2_SOURCES}
	${ost_SOURCES}
	${abi_SOURCES}
	${devices_SOURCES}
	${dev_pic_SOURCES}
	${dev_vga_SOURCES}
	${kernel_SOURCES}
	${basalt_SOURCES}
)

# Compiler-independent configuration

# CMake 3.5.0 does not support CXX_STANDARD 17
set_property(TARGET ${kernel_binary_name} PROPERTY C_STANDARD 11)
set_property(TARGET ${kernel_binary_name} PROPERTY C_STANDARD_REQUIRED TRUE)
set_property(TARGET ${kernel_binary_name} PROPERTY C_EXTENSIONS FALSE)
target_include_directories(${kernel_binary_name} PUBLIC ${CONFIG_INCLUDE_DIRS})
target_include_directories(${kernel_binary_name} PUBLIC ${CONFIG_DIR})


# Compiler-dependent configuration

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
	# common options
	target_compile_options(${kernel_binary_name} PUBLIC
		-ffreestanding
		-nostdlib
		-m32 -march=i686
	)

	target_compile_options(${kernel_binary_name} PUBLIC
		-Wall
		-Werror
		-Winline
		-Wextra
		-Wmissing-include-dirs
		-Wnull-dereference
		-Wstrict-aliasing
		-Wpedantic
		-Wcast-qual
		-Wcast-align
		-Wwrite-strings
		-Wlogical-op
		-Wlogical-not-parentheses
		-Wno-aggressive-loop-optimizations
		-Winline
		-Wdisabled-optimization
		-pedantic
		-pedantic-errors
	)

	# CXX options
	set_property(TARGET ${kernel_binary_name} APPEND PROPERTY CXX_COMPILE_OPTIONS
		-std=c++17
		-Weffc++
		-Wold-style-cast
		-Wsuggest-final-methods
		-Wsuggest-override
		-Wplacement-new=2
		-Wzero-as-null-pointer-constant
		-Wuseless-cast
		-fno-exceptions
		-fno-rtti
	)

	target_compile_options(${kernel_binary_name} PUBLIC
		$<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:${kernel_binary_name},CXX_COMPILE_OPTIONS>>)

	# C options

	# Link flags
	set_target_properties(${kernel_binary_name} PROPERTIES
		LINK_FLAGS " -T ${kernel_linker} -nostdlib")
	set_property(TARGET ${kernel_binary_name} APPEND PROPERTY LINK_DEPENDS ${kernel_linker})
else (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
	message(ERROR "Only GNU compilers are currently supported")
endif (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)



add_custom_target(start_qemu
	COMMENT "Starting kernel in Qemu..."
	COMMAND qemu-system-i386 -kernel ${kernel_binary_name})
add_dependencies(start_qemu ${kernel_binary_name})
