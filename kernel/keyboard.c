#include <bolgenos-ng/keyboard.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/ps_2.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>

static probe_ret_t ps2_keyboard_probe(ps2_line_t idx);
static void ps2_keyboard_handle_irq();

struct ps2_dev ps2_keyboard = {
	.probe			= ps2_keyboard_probe,
	.irq_handler		= ps2_keyboard_handle_irq
};

static probe_ret_t ps2_keyboard_probe(ps2_line_t idx) {
	(void) idx;
	//FIXME: add handler!
	return probe_ok;
}

static void ps2_keyboard_handle_irq() {
	uint8_t byte = ps2_receive_byte();
	char info[100];
	snprintf(info, 100, "got '%lu' from keyboard PS/2\n",
			(long unsigned) byte);
	vga_console_puts(info);
}

void ps2_keyboard_init() {
	ps2_register_device(&ps2_keyboard);
}
