#include <bolgenos-ng/keyboard.hpp>


namespace {


using namespace ps2::keyboard;

int __is_letter(ps2::keyboard::kb_key key) {
	return (kb_key_a <= key && key <= kb_key_z);
}

int __is_digit(ps2::keyboard::kb_key key) {
	return (kb_key_0 <= key && key <= kb_key_9);
}


} // namespace


ps2::keyboard::kb_key& ps2::keyboard::operator++(kb_key &key) {
	int num_val = static_cast<int>(key);
	key = static_cast<kb_key>(++num_val);
	return key;
}


char ps2::keyboard::to_printable_key(kb_key key) {
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

char ps2::keyboard::apply_lshift(char sym) {
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

