SECTIONS
{
	. = 0x0;
	kernel :
	{
		*(.text)
		*(.data)
	}
}
