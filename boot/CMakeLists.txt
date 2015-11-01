project(boot ASM-ATT)

set(linker_script_src "${CMAKE_CURRENT_SOURCE_DIR}/bootloader_mbr.ld")
set(linker_script "linker_script")
add_custom_target(${linker_script} SOURCES "${linker_script_src}")

set(dd "dd" "status=none")
set(zeros "/dev/zero")

set(mbr_code_elf "mbr_bootloader.elf")
add_executable(${mbr_code_elf} bootloader_mbr.s)
set_target_properties(${mbr_code_elf} PROPERTIES
	LINK_FLAGS "-T ${linker_script_src}")
add_dependencies(${mbr_code_elf} ${linker_script})


set(dummy_os_elf "dummy_os.elf")
add_executable(${dummy_os_elf} test_os_loader.s)
set_target_properties(${dummy_os_elf} PROPERTIES
	LINK_FLAGS "-T ${linker_script_src}")
add_dependencies(${dummy_os_elf} ${linker_script})


set(mbr_code_bin "mbr_code.bin")
add_custom_target(${mbr_code_bin}
	COMMAND objcopy -O binary -j boot_code ${mbr_code_elf} ${mbr_code_bin})
add_dependencies(${mbr_code_bin} ${mbr_code_elf})


set(dummy_os_bin "dummy_os.bin")
add_custom_target(${dummy_os_bin}
	COMMAND objcopy -O binary -j boot_code ${dummy_os_elf} ${dummy_os_bin})
add_dependencies(${dummy_os_bin} ${dummy_os_elf})

set(partition_table "partition_table.bin")
add_custom_target(${partition_table}
	COMMAND tools::gen_part_table ${partition_table})
add_dependencies(${partition_table} tools::gen_part_table)

set(mbr_image "mbr.img")
add_custom_target(${mbr_image}
	COMMAND ${dd} if=${zeros} of=${mbr_image} bs=512 count=1
	COMMAND ${dd} conv=notrunc if=${mbr_code_bin} of=${mbr_image}
	COMMAND ${dd} conv=notrunc bs=1 seek=446
		if=${partition_table} of=${mbr_image}
	COMMAND /bin/echo -en \"\\x55\\xaa\" | ${dd} conv=notrunc bs=1 seek=510
		of=${mbr_image}
)
add_dependencies(${mbr_image} ${mbr_code_bin} ${partition_table})



set(dummy_os_image "dummy_os.img")
add_custom_target(${dummy_os_image}
	COMMAND ${dd} if=${zeros} of=${dummy_os_image} bs=512 count=1
	COMMAND ${dd} conv=notrunc if=${dummy_os_bin} of=${dummy_os_image}
	COMMAND /bin/echo -en \"\\x55\\xaa\" | ${dd} conv=notrunc bs=1 seek=510
		of=${dummy_os_image}
)
add_dependencies(${dummy_os_image} ${dummy_os_bin})


set(disk_image "disk.img")
add_custom_target(${disk_image} ALL
	COMMAND ${dd} if=${mbr_image} of=${disk_image}
	COMMAND ${dd} oflag=append conv=notrunc if=${dummy_os_image} of=${disk_image}
)
add_dependencies(${disk_image} ${mbr_image} ${dummy_os_image})

# Runtime targets

set(start_target "start")
add_custom_target(${start_target}
	COMMAND kvm -hda ${disk_image}
)
add_dependencies(${start_target} ${disk_image})

set(debug_target "debug")
set(vm_startup_time 2)

set(debug_parameters_src "${CMAKE_CURRENT_SOURCE_DIR}/debug_parameters.gdb")
set(debug_parameters "debug_parameters")
add_custom_target(${debug_parameters} SOURCES "${debug_parameters_src}")

add_custom_target(${debug_target}
	COMMAND kvm -s -S -hda ${disk_image} &
	COMMAND sleep ${vm_startup_time}
	COMMAND echo ${debug_command}
	COMMAND xterm -hold -e gdb\ -ex\ 'source\ ${debug_parameters_src}'
)
add_dependencies(${debug_target} ${disk_image} ${debug_parameters})
