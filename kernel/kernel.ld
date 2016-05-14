ENTRY(_start_kernel)

SECTIONS {
	. = 0x100000; /* Kernel load address */
	__object_begin = .;

	. = ALIGN(4);
	/* multiboot header should be in first 8kb and aligned by 4 */
	.text : {
		*(.multiboot_header);
		__text_begin = .;
		*(.text*)
		__text_end = .;
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
		__stack_begin = .;
		*(.kernel_stack)
		__stack_end = .;
	}
	__object_end = .;
}
