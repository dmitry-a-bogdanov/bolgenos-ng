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

file(GLOB_RECURSE m4_input_files "m4/*")
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


file(GLOB_RECURSE bolgenos_HEADERS ${kernel_headers_dir}/*)
source_group("include" FILES ${bolgenos_HEADERS})


file(GLOB_RECURSE libkernelcxx_SOURCES libkernelcxx/*)
source_group("libkernelcxx" FILES ${libkernelcxx_SOURCES})

file(GLOB_RECURSE alloc_SOURCES alloc/*)
source_group("Allocators" FILES ${alloc_SOURCES})


file(GLOB_RECURSE ps2_SOURCES ps2/*)
source_group("ps2" FILES ${ps2_SOURCES})


file(GLOB_RECURSE lib_SOURCES lib/*)
source_group("lib" FILES ${lib_SOURCES})


file(GLOB_RECURSE ost_SOURCES ost/*)
source_group("ost" FILES ${ost_SOURCES})


file(GLOB_RECURSE abi_SOURCES abi/*)
source_group("abi" FILES ${abi_SOURCES})


file(GLOB devices_SOURCES devices/*)
source_group("devices" FILES ${devices_SOURCES})


file(GLOB_RECURSE dev_pic_SOURCES devices/interrupt_controller/*)
source_group("devices/PICs" FILES ${dev_pic_SOURCES})

file(GLOB_RECURSE dev_vga_SOURCES devices/vga/*)
source_group("devices/vga" FILES ${dev_vga_SOURCES})


file(GLOB kernel_SOURCES *)
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
)

# Compiler-independent configuration

# CMake 3.5.0 does not support CXX_STANDARD 17
set_property(TARGET ${kernel_binary_name} PROPERTY C_STANDARD 99)
set_property(TARGET ${kernel_binary_name} PROPERTY C_STANDARD_REQUIRED TRUE)
set_property(TARGET ${kernel_binary_name} PROPERTY C_EXTENSIONS FALSE)
target_include_directories(${kernel_binary_name} PUBLIC ${CONFIG_INCLUDE_DIRS})


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
