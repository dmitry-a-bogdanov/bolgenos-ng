#include "ps2_keyboard_sm.h"

#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>

char codeset2_ascii[256] = {
	  0,   0,    0,   0,      0,    0,    0, 0,		// 0x00 - 0x07
	  0,   0,    0,   0,      0, '\t',  '`', 0,		// 0x08 - 0x0f
	  0,   0,    0,   0,      0,  'Q',  '1', 0,		// 0x10 - 0x17
	  0,   0,  'Z', 'S',    'A',  'W',  '2', 0,		// 0x18 - 0x1f
	  0, 'C',  'X', 'D',    'E',  '4',  '3', 0,		// 0x20 - 0x27
	  0, ' ',  'V', 'F',    'T',  'R',  '5', 0,		// 0x28 - 0x2f
	  0, 'N',  'B', 'H',    'G',  'Y',  '6', 0,		// 0x30 - 0x37
	  0,   0,  'M', 'J',    'U',  '7',  '8', 0,		// 0x38 - 0x3f
	  0, ',',  'K', 'I',    'O',  '0',  '9', 0,		// 0x40 - 0x47
	  0, '.',  '/', 'L',    ';',  'P',  '-', 0,		// 0x48 - 0x4f
	  0,   0, '\'',   0,    '[',  '=',    0, 0,		// 0x50 - 0x57
	  0,   0, '\n', ']',      0, '\\',    0, 0,		// 0x58 - 0x5f
	  0,   0,    0,   0,      0,    0, '\b', 0,		// 0x60 - 0x67
	  0, '1',    0, '4',    '7',    0,    0, 0,		// 0x68 - 0x6f
	'0', '.',  '2', '5',    '6',  '8',    0, 0,		// 0x70 - 0x77
	  0, '+',  '3', '-',    '*',  '9',    0, 0,		// 0x78 - 0x7f

	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};


#define __release_byte			240
void ps2_kb_sm_put_byte(uint8_t byte) {
	//char info[100];
	//snprintf(info, 100, "got '%lu' from keyboard PS/2\n",
	//		(long unsigned) byte);
	//vga_console_puts(info);
	static int skip_next = 0;
	if (skip_next) {
		skip_next = 0;
		return;
	}
	if (byte == __release_byte) {
		skip_next = 1;
		return;
	}
	char c = codeset2_ascii[byte];
	if (c != 0) {
		vga_console_putc(c);
	}
}
