project(kernel ASM-ATT)
enable_language(C)
enable_language(ASM-ATT)

include(kernel_common)

set(CMAKE_ASM-ATT_SOURCE_FILE_EXTENSIONS s;sx;S)
# Use gcc for preprocessed GAS
set(CMAKE_ASM-ATT_COMPILER "${CMAKE_C_COMPILER}")
set(CMAKE_ASM-ATT_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_ASM-ATT_COMPILE_OBJECT ${CMAKE_C_COMPILE_OBJECT})

set(kernel_export_dir "kernel/")

set(kernel_linker "${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld")

include_directories("${CMAKE_SOURCE_DIR}/include")

add_definitions(-nostdlib -nostdinc)

set(kernel_elf "kernel_elf")
set_source_files_properties(boot.sx PROPERTIES
	LANGUAGE "ASM-ATT")

add_linked_executable(${kernel_elf} ${kernel_linker} boot.sx)

install(TARGETS ${kernel_elf} EXPORT ${kernel_elf}
	RUNTIME
	DESTINATION ${kernel_export_dir})
install(EXPORT ${kernel_elf} NAMESPACE kernel:: DESTINATION ${kernel_export_dir})

