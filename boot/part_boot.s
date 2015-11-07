.code16
.text
.set PART_BOOT_SEGMENT,	0x7c00	# part_boot.img is loaded to 0x0000:0x7c00
.org 0x0

.set KERNEL_SEGMENT,	0x0800	# segment for kernel.img
.set KERNEL_OFFSET,	0x0000	# offset in segment for kernel.img

.set KERNEL_IMG_ADDR,		(PART_BOOT_SEGMENT + 490)
.set KERNEL_IMG_HEADS_ADDR,	(KERNEL_IMG_ADDR + 0)
.set KERNEL_IMG_SECTORS_ADDR,	(KERNEL_IMG_ADDR + 1)
.set KERNEL_IMG_CYLINDERS_ADDR,	(KERNEL_IMG_ADDR + 2)
.set KERNEL_IMG_SIZE_ADDR,	(KERNEL_IMG_ADDR + 3)

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
	movb %dl, boot_device
	leaw status_part_boot_img_loaded, %si
	call puts

read_kernel_img_info:
	movb KERNEL_IMG_CYLINDERS_ADDR, %ch
	movb KERNEL_IMG_SECTORS_ADDR, %cl
	movb KERNEL_IMG_HEADS_ADDR, %dh
	movb KERNEL_IMG_SIZE_ADDR, %al

load_kernel_img:
	movb boot_device, %dl
	movb $2, %ah
	movw $(KERNEL_SEGMENT), %bx
	movw %bx, %es
	movw $(KERNEL_OFFSET), %bx
	int $0x13
	jc load_kernel_img_failure

	leaw status_kernel_img_loaded, %si
	call puts
	jmp start_kernel_img

.quad 0xeeeeeeeeeeeeeeee

start_kernel_img:
	ljmp $KERNEL_SEGMENT, $KERNEL_OFFSET

load_kernel_img_failure:
	leaw error_load_kernel_img, %si
	call puts
	jmp hang

hang:
	jmp hang


.data
boot_device:			.byte 0

error_load_kernel_img:		.asciz "loading kernel.img failure\n\r"

status_part_boot_img_loaded:	.asciz "part_boot.img started\n\r"
status_kernel_img_loaded:	.asciz "kernel.img loaded\n\r"

