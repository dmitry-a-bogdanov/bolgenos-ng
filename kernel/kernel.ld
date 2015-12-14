ENTRY(_start_kernel)

SECTIONS {
	. = 0x100000; /* Kernel load address */
	__ld_kernel_memory_begin = .;

	. = ALIGN(4);
	/* multiboot header should be in first 8kb and aligned by 4 */
	.text : {
		*(.multiboot_header);
		*(.text)
	}

	.data : {
		*(.data)
	}

	.bss :
	{
		*(.bss)
		*(.kernel_stack)
	}
	__ld_kernel_memory_end = .;
}
