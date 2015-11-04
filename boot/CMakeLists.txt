project(boot ASM-ATT)
enable_language(ASM-ATT)

set(linker_script_src "${CMAKE_CURRENT_SOURCE_DIR}/flat_binary.ld")
set(linker_script "linker_script")
add_custom_target(${linker_script} SOURCES "${linker_script_src}")

set(part_boot_linker_src "${CMAKE_CURRENT_SOURCE_DIR}/part_boot.ld")
set(part_boot_linker "part_boot_linker")
add_custom_target(${part_boot_linker} SOURCES "${part_boot_linker_src}")


set(dd "dd" "status=none")
set(zeros "/dev/zero")

set(boot_elf "boot.elf")
add_executable(${boot_elf} boot.s)
set_target_properties(${boot_elf} PROPERTIES
	LINK_FLAGS "-T ${linker_script_src}")
add_dependencies(${boot_elf} ${linker_script})


set(part_boot_elf "part_boot.elf")
add_executable(${part_boot_elf} part_boot.s)
set_target_properties(${part_boot_elf} PROPERTIES
	LINK_FLAGS "-T ${part_boot_linker_src}")
add_dependencies(${part_boot_elf} ${part_boot_linker})


set(boot_bin "boot.bin")
add_custom_target(${boot_bin}
	COMMAND objcopy -O binary -j boot_code ${boot_elf} ${boot_bin})
add_dependencies(${boot_bin} ${boot_elf})


set(part_boot_bin "part_boot.bin")
add_custom_target(${part_boot_bin}
	COMMAND objcopy -O binary -j boot_code ${part_boot_elf} ${part_boot_bin})
add_dependencies(${part_boot_bin} ${part_boot_elf})

set(partition_table "partition_table.bin")
add_custom_target(${partition_table}
	COMMAND tools::gen_part_table ${partition_table})
add_dependencies(${partition_table} tools::gen_part_table)

set(boot_image "boot.img")
add_custom_target(${boot_image}
	COMMAND ${dd} if=${zeros} of=${boot_image} bs=512 count=1
	COMMAND ${dd} conv=notrunc if=${boot_bin} of=${boot_image}
	COMMAND ${dd} conv=notrunc bs=1 seek=446
		if=${partition_table} of=${boot_image}
	COMMAND /bin/echo -en \"\\x55\\xaa\" | ${dd} conv=notrunc bs=1 seek=510
		of=${boot_image}
)
add_dependencies(${boot_image} ${boot_bin} ${partition_table})



set(part_boot_image "part_boot.img")
add_custom_target(${part_boot_image}
	COMMAND ${dd} if=${zeros} of=${part_boot_image} bs=512 count=10
	COMMAND ${dd} conv=notrunc if=${part_boot_bin} of=${part_boot_image}
	COMMAND /bin/echo -en \"\\x55\\xaa\" | ${dd} conv=notrunc bs=1 seek=510
		of=${part_boot_image}
	COMMAND /bin/echo -en \"\\x00\\x03\\x00\\x01\" | ${dd} conv=notrunc
		bs=1 seek=490 of=${part_boot_image}
)
add_dependencies(${part_boot_image} ${part_boot_bin})

add_custom_target(kernel.bin ALL
	COMMAND objcopy -O binary -j boot_code ${CMAKE_BINARY_DIR}/kernel/kernel_elf kernel.bin)
add_dependencies(kernel.bin kernel_elf)

set(disk_image "hdd.img")
add_custom_target(${disk_image} ALL
	COMMAND ${dd} if=${boot_image} of=${disk_image}
	COMMAND ${dd} oflag=append conv=notrunc if=${part_boot_image} of=${disk_image}
	COMMAND ${dd} bs=512 seek=2 conv=notrunc count=254 if=${zeros} of=${disk_image}
	COMMAND ${dd} bs=512 seek=2 conv=notrunc if=kernel.bin of=${disk_image}
)
add_dependencies(${disk_image} ${boot_image} ${part_boot_image} kernel.bin)

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
