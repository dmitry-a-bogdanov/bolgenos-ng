ENTRY(_start_kernel)

SECTIONS {
	. = 0x100000; /* Kernel load address */
	__kernel_obj_start = .;

	. = ALIGN(4);
	/* multiboot header should be in first 8kb and aligned by 4 */
	.text : {
		*(.multiboot_header);
		*(.text)
	}

	.ctors : {
		__ctors_begin = .;
		*(.ctors)
		__ctors_end = .;
	}

	.data : {
		*(.data)
	}

	.bss :
	{
		*(.bss)
		*(.kernel_stack)
	}
	__kernel_obj_end = .;
}
