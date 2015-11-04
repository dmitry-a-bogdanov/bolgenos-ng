project(kernel ASM-ATT)
enable_language(C)
enable_language(ASM-ATT)

message("dabogdanov: exts=${CMAKE_ASM-ATT_SOURCE_FILE_EXTENSIONS}")
set(CMAKE_ASM-ATT_SOURCE_FILE_EXTENSIONS s;sx;S)
set(CMAKE_ASM-ATT_COMPILER "${CMAKE_C_COMPILER}")
#set(CMAKE_ASM-ATT_COMPILE_OBJECT
#	"<CMAKE_ASM-ATT_COMPILER> <FLAGS> -c -o <OBJECT> <SOURCE>")
#set(CMAKE_ASM-ATT_LINK_EXECUTABLE ${CMAKE_C_LINK_EXECUTABLE})
set(kernel_export_dir "kernel/")

set(kernel_linker_src "${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld")
set(kernel_linker "kernel_linker")
add_custom_target(${kernel_linker} SOURCES "${kernel_linker_src}")

include_directories("${CMAKE_SOURCE_DIR}/include")

set(kernel_elf "kernel_elf")
set_source_files_properties(boot.sx PROPERTIES
	LANGUAGE "ASM-ATT"
	COMPILE_FLAGS "-c")
add_executable(${kernel_elf} boot.sx)
get_target_property(compile_rule ${kernel_elf} RULE_LAUNCH_COMPILE)
set_target_properties(${kernel_elf} PROPERTIES
	LINK_FLAGS "-T ${kernel_linker_src}")
add_dependencies(${kernel_elf} ${kernel_linker})


#set(kernel_bin "kernel.bin")
#add_custom_target(${kernel_bin}
#	COMMAND objcopy -O binary -j boot_code ${kernel_elf} ${kernel_bin})
#add_dependencies(${kernel_bin} ${kernel_elf})

install(TARGETS ${kernel_elf} EXPORT ${kernel_elf}
	RUNTIME
	DESTINATION ${kernel_export_dir})
install(EXPORT ${kernel_elf} NAMESPACE kernel:: DESTINATION ${kernel_export_dir})

#add_custom_target(kernel::${kernel_bin} ALIAS ${kernel_bin})


