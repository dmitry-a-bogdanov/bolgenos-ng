#include "ps2_keyboard.hpp"

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/keyboard.h>
#include <bolgenos-ng/string.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/ps2.hpp>

#include "ps2_keyboard_sm.hpp"

using namespace ps2;

class ps2_keyboard: public ps2::ps2_dev {
public:
	virtual probe_ret_t probe(ps2::line_t line);
	virtual void handle_irq();
	virtual ~ps2_keyboard() {};
};

enum ps2_kbd_cmd_t: uint8_t {
	ps2_dcmd_enable_scan		= 0xf4,
	ps2_dcmd_disable_scan		= 0xf5,
	ps2_dcmd_identify		= 0xf2
};



#define ps2_keyboard_ack		(0xfa)

#define this_dev_id_len			2
static uint8_t this_dev_id[this_dev_id_len] = { 0xab, 0x83 };

probe_ret_t ps2_keyboard::probe(ps2::line_t line) {
	clean_buffer();
	ioret_t ret;
	int id_count = 0;
	ret = send_byte_with_ack(line, ps2_dcmd_disable_scan,
			ps2_keyboard_ack);
	if (ret != ioret_t::ok) {
		cio::cwarn << "failed to disable scan: "
			<< ps2::strerror(ret) << cio::endl;
		goto fail;
	}

	ret = send_byte_with_ack(line, ps2_dcmd_identify,
			ps2_keyboard_ack);
	if (ret != ioret_t::ok) {
		cio::cwarn << "failed to identify dev: "
			<< ps2::strerror(ret) << cio::endl;
		goto fail;
	}

	while(ps2::wait_for_input(5 /* ms */)) {
		uint8_t id_byte;
		id_byte = ps2::receive_byte();
		if (id_byte != this_dev_id[id_count]) {
			cio::cwarn << "got wrong id_byte = "
				<< id_count << ":" << id_byte
				<< cio::endl;
			goto fail;
		}
		++id_count;
		if (id_count == this_dev_id_len)
			goto ok;
	}
fail:
	cio::cwarn << "line " << line << ": "
		<< "probe as ps2_keyboard FAILED" << cio::endl;
	return probe_next;
ok:
	// leave in a SCANNING state!
	(void) ps2::send_byte_with_ack(line, ps2_dcmd_enable_scan,
			ps2_keyboard_ack);
	cio::cnotice << "line " << line << ": "
		<< "probe as ps2_keyboard PASSED" << cio::endl;
	return probe_ok;
}

void ps2_keyboard::handle_irq() {
	uint8_t byte = ps2::receive_byte();
	kbd_put_byte(byte);
}

static ps2_keyboard ps2_kbd;

void ps2::keyboard::init() {
	ps2::register_device(&ps2_kbd);
}
