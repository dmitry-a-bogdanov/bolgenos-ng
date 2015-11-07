# Copyright dmitry.a.bogdanov@gmail.com
# GAS (Gnu Assembler)
# AT&T syntax
# TODO: Check ways for smarter relocation. Addresses *_START looks ugly.
.code16
.text
.org 0x0

.set LOAD_START, 0x7c00
.set RELO_START, 0x0600
.set MBR_SIZE, 0x0200
.set BOOT_FLAG_OFFSET, 0x01fe
.set DEBUG, 0
.set PARTITION_ENTRY_SIZE, 16
.set PARTITION_TABLE_START, 0x1be
.set PARTITION_BOOT_FLAG_OFFSET, 0x0
.set PARTITION_BOOT_FLAG, 0x80

.global _start
_start:
	jmp main

.func puts
# Description:
#   Prints given string to console.
#
# Args:
#   %ds.%si points to string
#
puts:
	lodsb				# Load char from %ds.%si to %al.
	testb %al, %al			# Test current byte;
	jz ret_from_puts		# and finish writing if it's '\0'

# BIOS interrput 0x10:
# http://www.ctyme.com/intr/rb-0106.htm
# %ah = 0xe - "Teletype output";
# %al - character to write;
# %bh - page number; WTF?. Not needed;
# %bl - foreground color for graphic mode. Not needed;
	movb $0xe, %ah
	movw $0x2, %bx
	int $0x10

	jmp puts

ret_from_puts:
	ret
.endfunc

main:
setup_cpu:
	cli					# Disable interrupts.

	movb %dl, (LOAD_START + boot_device)	# Save boot drive.

	movw %cs, %ax				# Code segment has correct
	movw %ax, %ds				# value and we propagating this
	movw %ax, %es				# value to all other segment
	movw %ax, %ss				# registers.

# Further code relocation will should not affect any operations and should not
# be noted and handled by something else except relocation code.
# Thus it's easier to start with relocated stack intead of moving it.
	movw $RELO_START, %sp

# Now cpu is setup and we can enable interrupts and start doing some work.
	sti					# Enable inerrupts.


# Display "loading" message as big boy:)
	leaw (LOAD_START + status_loading), %si
	call puts


# Copy whole MBR segment (in disk terms) long-by-long to new location.
copy_myself:
# Clear copy direction flag.
	cld
	movw $LOAD_START, %si
	movw $RELO_START, %di
	movw $MBR_SIZE, %cx
	rep movsb			# copies %cs bytes %ds.%si -> %es.%di


# MBR is copied to new location $RELO_START
.if DEBUG
	leaw (RELO_START + status_mbr_copied), %si
	call puts
.endif

continue_myself:
	ljmp $0x0000, $(RELO_START + continue_point)

.if DEBUG
.long 0xeeeeeeee	# Just marker to find address for breakpoint
.endif

continue_point:
# Now we a running on new location

.if DEBUG
	leaw (RELO_START + status_long_jump_success), %si
	call puts
.endif

find_bootable_partition:
	movw $(RELO_START + PARTITION_TABLE_START + PARTITION_BOOT_FLAG_OFFSET), %bp
	movw $4, %cx

find_bootable_partition_loop:
	movw (%bp), %dx
# TODO: Loop looks for first bootable partition but according to standard
# for MBR it should check number of bootable partitions first and then
# if number there is more than one bootable partition it should raise
# invalid partition table error.
	cmpw $PARTITION_BOOT_FLAG, %dx
	je load_partition
	addw $(PARTITION_ENTRY_SIZE), %bp
	dec %cx
	jcxz check_os_loader_failure		# jump if CX is zero
	jmp find_bootable_partition_loop

# After loop %bp points to the partition_active flag [=begging partition entry]
# *_R_AF = "relative active flag"
.set CYLINDERS_LOW_OFFSET_R_AF, 3
.set SECTORS_OFFSET_R_AF, 2
.set HEADS_OFFSET_R_AF, 1
load_partition:

.if DEBUG
	leaw (RELO_START + status_trying_to_load), %si
	call puts
.endif

# BIOS interrupt 0x13
# http://www.ctyme.com/intr/rb-0607.htm
# %ah = 2 - "Read sectors into memory"
# %al - number of sectors to read
# %ch - low eight bits of cylinder number
# %cl - sector number (bits 0-5), high bits of cylinder number (bits 6-7)
# %dh - head number
# %dl - boot drive
# %es.%bx - data buffer
# Requires -hdachs option for running kvm
	movb $2, %ah
	movb $1, %al

# Load to CHS to registers
	movb CYLINDERS_LOW_OFFSET_R_AF(%bp), %ch
	movb SECTORS_OFFSET_R_AF(%bp), %cl
	movb HEADS_OFFSET_R_AF(%bp), %dh

	movb (RELO_START + boot_device), %dl
	movw $LOAD_START, %bx
	int $0x13

# CF is set if failure
	jc load_partition_panic

# Now we loaded bootloader of OS to start segment.
# Let's tell everyone about it:)
	leaw (RELO_START + status_kernel_loaded), %si
	call puts

check_os_loader:
	movw (LOAD_START + BOOT_FLAG_OFFSET), %ax
	cmpw $0xaa55, %ax
	jne check_os_loader_failure


check_os_loader_succeess:
# Now we loaded OS loader and verified boot flag.
# Let's start OS loader
start_kernel:
	movb (RELO_START + boot_device), %dl
	ljmp $0x0000, $0x7c00

# Should never come here but I'm not so experienced in asm.
	int $0x18

check_os_loader_failure:
	leaw (RELO_START + check_os_error), %si
	jmp panic

load_partition_panic:
	leaw (RELO_START + load_error), %si

# Panic assumes that address of message is placed into %si.
panic:
	call puts
	int $0x18

.data

# Variables section
boot_device:			.byte 0
bootable_partition:		.byte 0

# Constants section

# Error messages.
# Should be named error_* ?
check_os_error:			.asciz "missing operating system\n\r"
disk_error:			.asciz "disk error\n\r"
load_error:			.asciz "load error\n\r"

# Status messages.
status_kernel_loaded:		.asciz "part_boot.img loaded\n\r"
status_loading:			.asciz "boot.img started\n\r"
.if DEBUG
# To don't spend a lot of memory for debug messages.
status_mbr_copied:		.asciz "1."
status_long_jump_success:	.asciz "2."
status_trying_to_load:		.asciz "3."
.endif

