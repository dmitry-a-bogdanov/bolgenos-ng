SECTIONS
{
	. = 0x0;
	boot_code :
	{
		*(.text)
		*(.data)
		*(prot_mode)
	}
}
