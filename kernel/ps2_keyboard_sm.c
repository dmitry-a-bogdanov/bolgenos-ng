#include "ps2_keyboard_sm.h"

#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>


char codeset2_ascii[256] = {
	  0,   0,    0,   0,      0,    0,    0, 0,		// 0x00 - 0x07
	  0,   0,    0,   0,      0, '\t',  '`', 0,		// 0x08 - 0x0f
	  0,   0,    0,   0,      0,  'q',  '1', 0,		// 0x10 - 0x17
	  0,   0,  'z', 's',    'a',  'w',  '2', 0,		// 0x18 - 0x1f
	  0, 'c',  'x', 'd',    'e',  '4',  '3', 0,		// 0x20 - 0x27
	  0, ' ',  'v', 'f',    't',  'r',  '5', 0,		// 0x28 - 0x2f
	  0, 'n',  'b', 'h',    'g',  'y',  '6', 0,		// 0x30 - 0x37
	  0,   0,  'm', 'j',    'u',  '7',  '8', 0,		// 0x38 - 0x3f
	  0, ',',  'k', 'i',    'o',  '0',  '9', 0,		// 0x40 - 0x47
	  0, '.',  '/', 'l',    ';',  'p',  '-', 0,		// 0x48 - 0x4f
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

static char ps2_kbd_lshift(char sym) {
	if ('a' <= sym && sym <= 'z') {
		char offset = sym - 'a';
		return 'A' + offset;
	}
	switch (sym) {
	case '`':	return '~';
	case '1':	return '!';
	case '2':	return '@';
	case '3':	return '#';
	case '4':	return '$';
	case '5':	return '%';
	case '6':	return '^';
	case '7':	return '&';
	case '8':	return '*';
	case '9':	return '(';
	case '0':	return ')';
	case '-':	return '_';
	case '=':	return '+';
	case ',':	return '<';
	case '.':	return '>';
	case '/':	return '?';
	case '[':	return '{';
	case ']':	return '}';
	case '\\':	return '|';
	case ';':	return ':';
	default:	return sym;
	}
}


#define __release_byte			240
#define __left_shift_byte		0x12
void ps2_kb_sm_put_byte(uint8_t byte) {
	//char info[100];
	//snprintf(info, 100, "got '%lu' from keyboard PS/2\n",
	//		(long unsigned) byte);
	//vga_console_puts(info);
	static int skip_next = 0;
	static int left_shift = 0;
	if (skip_next) {
		if (byte == 0x12)
			left_shift = 0;
		skip_next = 0;
		return;
	}
	if (byte == __release_byte) {
		skip_next = 1;
		return;
	}
	if (byte == __left_shift_byte) {
		left_shift = 1;
	}
	char c = codeset2_ascii[byte];
	if (left_shift)
		c = ps2_kbd_lshift(c);
	if (c != 0) {
		vga_console_putc(c);
	}
}
