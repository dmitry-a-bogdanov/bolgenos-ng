#pragma once

#include <bolgenos-ng/ps2/device.hpp>
#include <bolgenos-ng/keyboard.h>

#include "ps2_keyboard_sm.hpp"


namespace ps2 {

namespace keyboard {

enum class key_status_t {
	pressed,
	released
};

class PS2DefaultKeyboard: public ps2::IPS2Device {
public:
	PS2DefaultKeyboard()
			: _sm(this)
	{
		for (auto& status : key_statuses_) {
			status = key_status_t::released;
		}
	}

	probe_ret_t probe(ps2::IPS2Line *line) override;
	irq::irq_return_t handle_irq() override;
	virtual ~PS2DefaultKeyboard() {};


	key_status_t& key(uint8_t idx)
	{
		return key_statuses_[idx];
	}
private:
	ps2_kbd_impl::KbdStateMachine _sm;
	key_status_t key_statuses_[__kb_key_max];
};

// \brief Initilize PS/2 keyboard driver.
//
// Fuction registers PS/2 keyboard in PS/2 susbsystem.
void init();

} // namespace keyboard

} // namespace ps2