#include "ps2_keyboard.hpp"

#include <bolgenos-ng/log.hpp>
#include <ps2/device.hpp>
#include <ps2/controller.hpp>
#include <ps2/line.hpp>


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
		cwarn	<< "failed to disable scan: " << ret << endl;
		goto fail;
	}

	ret = line->send(ps2_dcmd_identify, ps2_keyboard_ack);
	if (ret != ErrorCode::ok) {
		cwarn	<< "failed to identify device: " << ret << endl;
		goto fail;
	}

	uint8_t reply;
	while(line->controller()->receive(5, &reply)) {
		if (reply != this_dev_id[id_count]) {
			lib::cwarn << "got wrong id byte[" << id_count << "] = "
					<< reply << lib::endl;
			goto fail;
		}
		++id_count;
		if (id_count == this_dev_id_len)
			goto ok;
	}
fail:
	cwarn << "line " << line->id() << ": probe as ps2_keyboard FAILED" << endl;
	return probe_next;
ok:
	// leave in a SCANNING state!
	(void) line->send(ps2_dcmd_enable_scan, ps2_keyboard_ack);
	cnotice << "line " << line->id() << ": probe as ps2_keyboard PASSED" << endl;
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
