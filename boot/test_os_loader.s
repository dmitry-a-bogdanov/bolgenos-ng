.code16
.intel_syntax
.text
.org 0x0

LOAD_START = 0x7c00
RELO_START = 0x0600
MBR_SIZE = 0x0200

.global _start
_start:
	jmp main

.func put_string
put_string:
	lodsb			# load byte at ds:si into al (advancing si)
	or %al, %al		# test if character is 0 (end)
	jz put_string_done	# jump to end if 0.

	mov %ah, 0xe		# Subfunction 0xe of int 10h (video teletype output)
	mov %bx, 0xa
	int 0x10		# call BIOS interrupt.

	jmp put_string		# Repeat for next character.

put_string_done:
	ret
.endfunc

.func reboot
reboot:
	lea %si, [LOAD_START + rebootmsg]	# Load address of reboot message into si
	call put_string		# print the string
	xor %ax, %ax		# subfuction 0
	int 0x16		# call bios to wait for key
	ljmp 0xFFFF, 0x0000	# jump to FFFF:0000 is restart
.endfunc

main:
	# Setup segments:
	cli
	mov %ax, %cs			# CS = 0x0, since that's where boot sector is (0x07c00)
	mov %ds, %ax			# DS = CS = 0x0
	mov %es, %ax			# ES = CS = 0x0
	mov %ss, %ax			# SS = CS = 0x0
	mov %sp, LOAD_START			# Stack grows down from offset 0x7C00 toward 0x0000.
	sti

	# Display "loading" message:
	lea %si, [LOAD_START + load_message]
	call put_string

	call reboot


#.global bootloader_bss
_boot__boot_drive:		.byte 0
bootable_partition:		.byte 0

#.global bootloader_data

load_message:		.asciz "TEST OS STARTED\n\r"

rebootmsg:		.asciz "press any key to reboot\n\r"
