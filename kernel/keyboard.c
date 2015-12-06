#include <bolgenos-ng/keyboard.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/ps_2.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>

#include "ps2_keyboard_sm.h"

static probe_ret_t ps2_keyboard_probe(ps2_line_t line);
static void ps2_keyboard_handle_irq();

struct ps2_dev ps2_keyboard = {
	.probe			= ps2_keyboard_probe,
	.irq_handler		= ps2_keyboard_handle_irq
};

enum {
	ps2_dcmd_enable_scan		= 0xf4,
	ps2_dcmd_disable_scan		= 0xf5,
	ps2_dcmd_identify		= 0xf2
} ps2_dev_cmd_t;

#define ps2_keyboard_ack		(0xfa)

#define this_dev_id_len			2
static uint8_t this_dev_id[this_dev_id_len] = { 0xab, 0x83 };

static probe_ret_t ps2_keyboard_probe(ps2_line_t line) {
	char info[100];
	ps2_clean_buffer();
	ps2_ioret_t ret;
	ret = ps2_send_byte_with_ack(line, ps2_dcmd_disable_scan,
			ps2_keyboard_ack);
	if (ret != ps2_ioret_ok) {
		snprintf(info, 100, "failed to disable scan: %s\n",
				ps2_ioret_strerror(ret));
		vga_console_puts(info);
		goto fail;
	}

	ret = ps2_send_byte_with_ack(line, ps2_dcmd_identify,
			ps2_keyboard_ack);
	if (ret != ps2_ioret_ok) {
		snprintf(info, 100, "failed to identify dev: %s\n");
		vga_console_puts(info);
		goto fail;
	}

	int id_count = 0;
	while(ps2_wait_for_input(5,5)) {
		uint8_t id_byte;
		id_byte = ps2_receive_byte();
		if (id_byte != this_dev_id[id_count]) {
			snprintf(info, 100, "got wrong id_byte = %li:%lu\n",
				(long) id_count, id_byte);
			vga_console_puts(info);
			goto fail;
		}
		++id_count;
		if (id_count == this_dev_id_len)
			goto ok;
	}
fail:
	snprintf(info, 100, "line %li: probe as ps2_keyboard FAILED\n",
			(long) line);
	vga_console_puts(info);
	return probe_next;
ok:
	// leave in a SCANNING state!
	(void) ps2_send_byte_with_ack(line, ps2_dcmd_enable_scan,
			ps2_keyboard_ack);
	snprintf(info, 100, "line %li: probe as ps2_keyboard PASSED\n",
			(long) line);
	vga_console_puts(info);
	return probe_ok;
}

static void ps2_keyboard_handle_irq() {
	while (ps2_can_read()) {
		uint8_t byte = ps2_receive_byte();
		ps2_kb_sm_put_byte(byte);
	}
}

void ps2_keyboard_init() {
	ps2_register_device(&ps2_keyboard);
}
