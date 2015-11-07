
# definitions

set(BOOTABLE_FLAG \"\\x55\\xaa\")
set(DD "dd" "status=none" "conv=notrunc")

# functions

function(add_flat_executable elf_name ld_script elf_section)
	add_executable(${elf_name} ${ARGN})
	set(ld_script_target "${elf_name}_linker")
	add_custom_target(${ld_script_target} SOURCES ${ld_script})
	set_target_properties(${elf_name} PROPERTIES
		LINK_FLAGS "-T ${ld_script}")
	add_dependencies(${elf_name} ${ld_script_target})
	add_custom_command(TARGET ${elf_name}
		POST_BUILD
		COMMAND objcopy --output-target binary
				--only-section ${elf_section}
				${elf_name} ${elf_name}
		COMMENT "rewriting ${elf_name} by section ${elf_section}")
endfunction(add_flat_executable)

