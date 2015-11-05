project(boot ASM-ATT)
enable_language(ASM-ATT)

include(kernel_common)

set(boot_elf "boot.elf")
set(boot_bin "boot.bin")
set(boot_linker "${CMAKE_CURRENT_SOURCE_DIR}/boot.ld")
add_linked_executable(${boot_elf} ${boot_linker} boot.s)
add_flat_binary(${boot_bin} ${boot_elf} boot_code)

set(part_boot_elf "part_boot.elf")
set(part_boot_bin "part_boot.bin")
set(part_boot_linker "${CMAKE_CURRENT_SOURCE_DIR}/part_boot.ld")
add_linked_executable(${part_boot_elf} ${part_boot_linker} part_boot.s)
add_flat_binary(${part_boot_bin} ${part_boot_elf} boot_code)


set(partition_table "partition_table.bin")
add_custom_target(${partition_table}
	COMMAND tools::gen_part_table ${partition_table})
add_dependencies(${partition_table} tools::gen_part_table)

set(boot_image "boot.img")
add_custom_target(${boot_image}
	COMMAND ${DD} if=/dev/zero of=${boot_image} bs=512 count=1
	COMMAND ${DD} if=${boot_bin} of=${boot_image}
	COMMAND ${DD} bs=1 seek=446
		if=${partition_table} of=${boot_image}
	COMMAND /bin/echo -en ${BOOTABLE_FLAG} | ${DD} bs=1 seek=510
		of=${boot_image}
)
add_dependencies(${boot_image} ${boot_bin} ${partition_table})



set(part_boot_image "part_boot.img")
add_custom_target(${part_boot_image}
	COMMAND ${DD} if=/dev/zero of=${part_boot_image} bs=512 count=10
	COMMAND ${DD} if=${part_boot_bin} of=${part_boot_image}
	COMMAND /bin/echo -en ${BOOTABLE_FLAG} | ${DD} bs=1 seek=510
		of=${part_boot_image}
	COMMAND /bin/echo -en \"\\x00\\x03\\x00\\x01\" | ${DD}
		bs=1 seek=490 of=${part_boot_image}
)
add_dependencies(${part_boot_image} ${part_boot_bin})

add_custom_target(kernel.bin ALL
	COMMAND objcopy -O binary -j boot_code ${CMAKE_BINARY_DIR}/kernel/kernel_elf kernel.bin)
add_dependencies(kernel.bin kernel_elf)

set(disk_image "hdd.img")
add_custom_target(${disk_image} ALL
	COMMAND ${DD} if=${boot_image} of=${disk_image}
	COMMAND ${DD} oflag=append if=${part_boot_image} of=${disk_image}
	COMMAND ${DD} bs=512 seek=2 count=254 if=/dev/zero of=${disk_image}
	COMMAND ${DD} bs=512 seek=2 if=kernel.bin of=${disk_image}
)
add_dependencies(${disk_image} ${boot_image} ${part_boot_image} kernel.bin)

# Runtime targets

set(start_target "start")
# Also update Bochs' disk geometry!
add_custom_target(${start_target}
	COMMAND kvm -hdachs 2,4,32 -hda ${disk_image}
)
add_dependencies(${start_target} ${disk_image})

set(debug_target "debug")
set(vm_startup_time 2)

set(debug_parameters_src "${CMAKE_CURRENT_SOURCE_DIR}/debug_parameters.gdb")
set(debug_parameters "debug_parameters")
add_custom_target(${debug_parameters} SOURCES "${debug_parameters_src}")

# Also update Bochs' disk geometry!
add_custom_target(${debug_target}
	COMMAND kvm -s -S -hdachs 2,4,32 -hda ${disk_image} &
	COMMAND sleep ${vm_startup_time}
	COMMAND echo ${debug_command}
	COMMAND xterm -hold -e gdb\ -ex\ 'source\ ${debug_parameters_src}'
)
add_dependencies(${debug_target} ${disk_image} ${debug_parameters})
