#include "ps2_keyboard_sm.hpp"

#include <bolgenos-ng/keyboard.hpp>
#include <bolgenos-ng/vga_console.hpp>
#include <bolgenos-ng/log.hpp>

#include "ps2_keyboard.hpp"

using namespace ps2_kbd_impl;
using namespace ps2::keyboard;


namespace {


char codeset2_1b_translation_table[256] = {
	// 0x00
	0, kb_key_f9, 0, kb_key_f5,
	kb_key_f3, kb_key_f1, kb_key_f2, kb_key_f12,
	0, kb_key_f10, kb_key_f8, kb_key_f6,
	0, kb_key_tab,  kb_key_back_tick, 0,


	// 0x10
	0, kb_key_lalt, kb_key_lshift, 0,
	kb_key_lctrl, kb_key_q, kb_key_1, 0,
	0, 0, kb_key_z, kb_key_s,
	kb_key_a, kb_key_w, kb_key_2, 0,

	// 0x20
	0, kb_key_c, kb_key_x, kb_key_d,
	kb_key_e, kb_key_4, kb_key_3, 0,
	0, kb_key_space, kb_key_v, kb_key_f,
	kb_key_t, kb_key_r, kb_key_5, 0,


	// 0x30
	0, kb_key_n, kb_key_b, kb_key_h,
	kb_key_g, kb_key_y, kb_key_6, 0,
	0,   0,  kb_key_m, kb_key_j,
	kb_key_u, kb_key_7, kb_key_8, 0,


	// 0x40
	0, kb_key_comma,  kb_key_k, kb_key_i,
	kb_key_o, kb_key_0, kb_key_9, 0,
	0, kb_key_dot, kb_key_slash, kb_key_l,
	kb_key_semicolon, kb_key_p, kb_key_minus, 0,

	// 0x50
	0,   0, kb_key_apos, 0,
	kb_key_sqbracketl, kb_key_equal, 0, 0,
	kb_key_capslock, kb_key_rshift, kb_key_enter, kb_key_sqbracketr,
	0, kb_key_backslash, 0, 0,

	// 0x60
	0, 0, 0, 0,
	0, 0, kb_key_backspace, 0,
	0, kb_key_num_1, 0, kb_key_num_4,
	kb_key_num_7, 0, 0, 0,

	// 0x70
	kb_key_num_0, kb_key_num_dot, kb_key_num_2, kb_key_num_5,
	kb_key_num_6, kb_key_num_8, kb_key_escape, kb_key_numlock,
	kb_key_f11, kb_key_num_plus, kb_key_num_3, kb_key_num_minus,
	kb_key_num_star, kb_key_num_9, kb_key_scrolllock, 0,

	// 0x80
	0, 0, 0, kb_key_f7,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};


constexpr uint8_t codeset2_1b_release_byte = 0xf0;


} // namespace


handle_status_t WaitState::handle_byte(uint8_t byte)
{
	if (byte == codeset2_1b_release_byte) {
		_machine->set_state(_machine->one_byte_key_released());
	} else if (codeset2_1b_translation_table[byte] != __kb_key_none) {
		_machine->set_state(_machine->one_byte_key_pressed());
	} else {
		_machine->set_state(_machine->error_state());
	}
	return handle_status_t::cont;
}


handle_status_t OneByteKeyPressedState::handle_byte(uint8_t byte)
{
	int key = codeset2_1b_translation_table[byte];
	if (key != __kb_key_none) {
		auto dev = _machine->get_device();
		if (dev->key(key) != key_status_t::pressed) {
			dev->key(key) = key_status_t::pressed;
			_machine->set_state(_machine->print_state());
		} else {
			_machine->set_state(_machine->wait_state());
		}
	}
	return handle_status_t::cont;
}


handle_status_t OneByteKeyReleasedState::handle_byte(uint8_t byte)
{
	if (byte == codeset2_1b_release_byte) {
		// got 0xf0. wait for next byte
		_machine->set_state(_machine->one_byte_key_released());
		return handle_status_t::done;
	}
	int key = codeset2_1b_translation_table[byte];
	if (key != __kb_key_none) {
		_machine->get_device()->key(key) = ps2::keyboard::key_status_t::released;
		_machine->set_state(_machine->wait_state());
		return handle_status_t::done;
	} else {
		_machine->set_state(_machine->error_state());
		return handle_status_t::cont;
	}
}


handle_status_t PrintState::handle_byte(uint8_t byte __attribute__((unused)))
{
	auto device = _machine->get_device();

	int lshift = (device->key(kb_key_lshift) == key_status_t::pressed);
	for (kb_key key = __kb_key_none; key < __kb_key_max; ++key) {
		if (device->key(key) ==  key_status_t::pressed) {
			char symbol = to_printable_key(key);
			if (symbol) {
				if (lshift) {
					symbol = apply_lshift(symbol);
				}
				vga_console::putc(symbol);
				device->key(key) = key_status_t::released;
			}
		}
	}

	_machine->set_state(_machine->wait_state());
	return handle_status_t::done;
}


handle_status_t ErrorState::handle_byte(uint8_t byte)
{
	lib::cerr << "PS/2 keyboard: unexpected byte from controller " << int(byte) << lib::endl;
	_machine->set_state(_machine->wait_state());
	return handle_status_t::done;
}
