#include <bolgenos-ng/keyboard.h>

#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/vga_console.h>


char kb_keys_pressed[__kb_key_max] = { KEY_RELEASED };

static int __is_letter(kb_key key) {
	return (kb_key_a <= key && key <= kb_key_z);
}

static int __is_digit(kb_key key) {
	return (kb_key_0 <= key && key <= kb_key_9);
}

static char printable_key(kb_key key) {
	if (__is_letter(key)) {
		return 'a' + (key - kb_key_a);
	}

	if (__is_digit(key)) {
		return '0' + (key - kb_key_0);
	}

	switch (key) {
	case kb_key_space:				return ' ';
	case kb_key_back_tick:				return '`';
	case kb_key_comma:				return ',';
	case kb_key_dot:				return '.';
	case kb_key_slash:				return '/';
	case kb_key_semicolon:				return ';';
	case kb_key_minus:				return '-';
	case kb_key_apos:				return '\'';
	case kb_key_sqbracketl:				return '[';
	case kb_key_equal:				return '=';
	case kb_key_enter:				return '\n';
	case kb_key_sqbracketr:				return ']';
	case kb_key_backslash:				return '\\';

	default:return 0;
	}
}

static char kb_lshift(char sym) {
	if ('a' <= sym && sym <= 'z') {
		char offset = sym - 'a';
		return 'A' + offset;
	}
	switch (sym) {
	case '0':	return ')';
	case '1':	return '!';
	case '2':	return '@';
	case '3':	return '#';
	case '4':	return '$';
	case '5':	return '%';
	case '6':	return '^';
	case '7':	return '&';
	case '8':	return '*';
	case '9':	return '(';
	case '`':	return '~';
	case ',':	return '<';
	case '.':	return '>';
	case '/':	return '?';
	case ';':	return ':';
	case '-':	return '_';
	case '\'':	return '"';
	case '[':	return '{';
	case ']':	return '}';
	case '=':	return '+';
	case '\\':	return '|';
	default:	return sym;
	}
}

void kb_print_to_vga() {
	int lshift = (kb_keys_pressed[kb_key_lshift] == KEY_PRESSED);
	for (kb_key key = __kb_key_none; key < __kb_key_max; ++key) {
		if (kb_keys_pressed[key] == KEY_PRESSED) {
			char symbol = printable_key(key);
			if (symbol) {
				if (lshift) {
					symbol = kb_lshift(symbol);
				}
				vga_console_putc(symbol);
				kb_keys_pressed[key] = KEY_RELEASED;
			}
		}
	}
}
