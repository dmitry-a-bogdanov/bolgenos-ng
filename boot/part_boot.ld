SECTIONS
{
	. = 0x7c00;
	boot_code :
	{
		*(.text)
		*(.data)
	}
}
