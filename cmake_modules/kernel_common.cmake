
# definitions

set(BOOTABLE_FLAG \"\\x55\\xaa\")
set(DD "dd" "status=none" "conv=notrunc")

# functions

function(add_linked_executable elf_name ld_script)
	add_executable(${elf_name} ${ARGN})
	set(ld_script_target "${elf_name}_linker")
	add_custom_target(${ld_script_target} SOURCES ${ld_script})
	set_target_properties(${elf_name} PROPERTIES
		LINK_FLAGS "-T ${ld_script}")
	add_dependencies(${elf_name} ${ld_script_target})
endfunction(add_linked_executable)

function(add_flat_binary output_flat elf_input elf_section )
	add_custom_target(${output_flat}
		COMMAND objcopy --output-target binary
			--only-section ${elf_section}
			${elf_input} ${output_flat})
	add_dependencies(${output_flat} ${elf_input})
endfunction(add_flat_binary)
