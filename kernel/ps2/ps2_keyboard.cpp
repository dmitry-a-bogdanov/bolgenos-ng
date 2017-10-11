#include "ps2_keyboard.hpp"

#include <cstring>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/keyboard.h>
#include <bolgenos-ng/ps2_controller.hpp>
#include <bolgenos-ng/ps2_line.hpp>
#include <bolgenos-ng/ps2/device.hpp>


#include "ps2_keyboard_sm.hpp"

using namespace lib;
using namespace ps2;


enum ps2_kbd_cmd_t: uint8_t {
	ps2_dcmd_enable_scan		= 0xf4,
	ps2_dcmd_disable_scan		= 0xf5,
	ps2_dcmd_identify		= 0xf2
};



#define ps2_keyboard_ack		(0xfa)

#define this_dev_id_len			2
static uint8_t this_dev_id[this_dev_id_len] = { 0xab, 0x83 };

probe_ret_t ps2::keyboard::PS2DefaultKeyboard::probe(ps2::IPS2Line* line) {

	line->controller()->clean_buffer();

	int id_count = 0;
	auto ret = line->send(ps2_dcmd_disable_scan, ps2_keyboard_ack);
	if (ret != ErrorCode::ok) {
		LOG_WARNING("failed to disable scan: " << ret);
		goto fail;
	}

	ret = line->send(ps2_dcmd_identify, ps2_keyboard_ack);
	if (ret != ErrorCode::ok) {
		LOG_WARNING("failed to identify device: " << ret);
		goto fail;
	}

	uint8_t reply;
	while(line->controller()->receive(5, &reply)) {
		if (reply != this_dev_id[id_count]) {
			LOG_WARNING("got wrong id byte[" << id_count << "] = " << reply);
			goto fail;
		}
		++id_count;
		if (id_count == this_dev_id_len)
			goto ok;
	}
fail:
	LOG_WARNING("line " << line->id() << ": probe as ps2_keyboard FAILED");
	return probe_next;
ok:
	// leave in a SCANNING state!
	(void) line->send(ps2_dcmd_enable_scan, ps2_keyboard_ack);
	LOG_NOTICE("line " << line->id() << ": probe as ps2_keyboard PASSED");
	return probe_ok;
}

irq::IRQHandler::status_t ps2::keyboard::PS2DefaultKeyboard::handle_irq() {
	uint8_t byte = ps2::PS2Controller::instance()->receive_byte();
	_sm.handle_byte(byte);
	return irq::IRQHandler::status_t::HANDLED;
}


void ps2::keyboard::init() {
	static ps2::keyboard::PS2DefaultKeyboard ps2_kbd;
	ps2::PS2Controller::instance()->register_driver(&ps2_kbd);
}
