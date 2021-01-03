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
		libgcc.a:(.text)
		__text_end = .;
	}

	.ctors : {
		__ctors_begin = .;
		*(.ctors)
		__ctors_end = .;
	}

	.rodata : {
		*(.rodata)
		*(.rodata.*)
	}

	.eh_frame_hdr : {
		*(.eh_frame_hdr)
		*(.eh_frame_hdr.*)
	}

	.eh_frame : {
		*(.eh_frame)
		*(.eh_frame.*)
	}

	.gcc_except_table : {
		*(.gcc_except_table)
		*(.gcc_except_table.*)
	}

	.data : {
		*(.data)
		*(.data*)
		libgcc.a:(.data)
	}

	.bss : {
		*(.bss)
		libgcc.a(.bss)
	}

	.kernel_stack : {
		__stack_begin = .;
		*(.kernel_stack)
		__stack_end = .;
		}

	.comment : {
		*(.comment)
		*(.comment.*)
	}


	__object_end = .;
}
