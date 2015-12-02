#include <bolgenos-ng/ps_2.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>

enum ps2_reply {
	ps2_rpl_self_test_ok	= 0x55,
	ps2_rpl_self_test_fail	= 0xfc,
	ps2_rpl_port_test_ok	= 0x0
};

// PS/2 status register bits
enum ps2_sr {
	ps2_sr_out_buf_status	= 1 << 0,
	ps2_sr_in_buf_status	= 1 << 1,
	ps2_sr_post_passed	= 1 << 2,
	ps2_sr_comm_or_data	= 1 << 3,
	// unknown		= 1 << 4,
	// unknown		= 1 << 5,
	ps2_sr_timeout		= 1 << 6,
	ps2_sr_parity_error	= 1 << 7
};

// PS/2 configuration byte bits
enum ps2_cb {
	ps2_cb_int_first	= 1 << 0,
	ps2_cb_int_second	= 1 << 1,
	ps2_cb_system		= 1 << 2,
	ps2_cb_clock_first	= 1 << 4,
	ps2_cb_clock_second	= 1 << 5,
	ps2_cb_translation	= 1 << 6
};

typedef enum {
	ps2_cmd_read_cb		= 0x20,
	ps2_cmd_write_cb	= 0x60,

	ps2_cmd_self_test	= 0xaa,
	ps2_cmd_port_test_1	= 0xab,
	ps2_cmd_port_test_2	= 0xa9,

	ps2_cmd_disable_1	= 0xad,
	ps2_cmd_enable_1	= 0xae,
	ps2_cmd_disable_2	= 0xa7,
	ps2_cmd_enable_2	= 0xa8
} ps2_command_t;

enum ps2_port {
	ps2_data_port		= 0x60,
	ps2_status_reg		= 0x64,
	ps2_command_reg		= 0x64
};

uint8_t ps2_receive_byte() {
	return inb(ps2_data_port);
}

void ps2_send_command(ps2_command_t cmd) {
	outb(ps2_command_reg, cmd);
}

static void ps2_enable_dev(ps2_line_t idx);
static void ps2_disable_dev(ps2_line_t idx);

#define MAX_PS2_KNOWN_DEVS 10
#define MAX_PS2_ACTIVE_DEVS 2

// TODO: replace array with list in order to avoid troubles with support
// of many types of keyboards or mice
static struct ps2_dev *ps2_known_devices[MAX_PS2_KNOWN_DEVS];
static int ps2_known_device_count = 0;

void ps2_register_device(struct ps2_dev *dev) {
	if (ps2_known_device_count < MAX_PS2_KNOWN_DEVS) {
		ps2_known_devices[ps2_known_device_count++] = dev;
	}
}

static struct ps2_dev *ps2_active_devices[__ps2_dev_max];

static uint8_t __ps2_read_conf_byte();
static void __ps2_write_conf_byte(uint8_t conf_byte);
static probe_ret_t __ps2_probe_devices();
static void __handle_ps2_interrupt(ps2_line_t line);
static void handle_first_ps2_dev_int(irq_t);
static void handle_second_ps2_dev_int(irq_t);

static void ps2_enable_dev(ps2_line_t idx) {
	if (idx == ps2_dev_1) {
		ps2_send_command(ps2_cmd_enable_1);
	} else {
		ps2_send_command(ps2_cmd_enable_2);
	}
}

static void ps2_disable_dev(ps2_line_t idx) {
	ps2_command_t cmd;
	if (idx == ps2_dev_1) {
		cmd = ps2_cmd_disable_1;
	} else {
		cmd = ps2_cmd_disable_2;
	}
	ps2_send_command(cmd);
}

#define __PS2_FIRST_LINE_IRQ		(min_pic_irq + 1)
#define __PS2_SECOND_LINE_IRQ		(min_pic_irq + 12)

void ps2_init() {
	char info[100];
	snprintf(info, 100, "initializing PS/2 controller...\n");
	vga_console_puts(info);
	ps2_disable_dev(ps2_dev_2);
	ps2_disable_dev(ps2_dev_1);

	//FIXME: flush chip's output buffer

	uint8_t configuration_byte = __ps2_read_conf_byte();

	snprintf(info, 100, "PS/2 conf byte=%lu\n",
		(unsigned long) configuration_byte);
	vga_console_puts(info);

	int ps2_ports;
	if (configuration_byte & ps2_cb_clock_second) {
		ps2_ports = 2;
	} else {
		ps2_ports = 1;
	}
	snprintf(info, 100, "this system has %lu PS/2 port(s)\n",
		(unsigned long) ps2_ports);
	vga_console_puts(info);

	// temporary disable interrupts from PS/2
	uint8_t disable_int_cb = configuration_byte & (~ps2_cb_int_first) &
			(~ps2_cb_int_second) & (~ps2_cb_translation);

	__ps2_write_conf_byte(disable_int_cb);

	// run self-test
	ps2_send_command(ps2_cmd_self_test);
	long tries = 0;
	// FIXME: handle self-test failure
	while (ps2_receive_byte() != ps2_rpl_self_test_ok) {
		snprintf(info, 100, "try #%li failed\n", tries);
		vga_console_puts(info);
		++tries;
	}
	snprintf(info, 100, "PS/2 controller passed self-test\n");
	vga_console_puts(info);

	// test PS/2 ports
	ps2_send_command(ps2_cmd_port_test_1);

	tries = 0;
	// FIXME handle failure
	// FIXME since this code related to PS/2 lines separately it's neeeded
	//	to move this code to probe function
	while (ps2_receive_byte() != ps2_rpl_port_test_ok) {
		snprintf(info, 100, "try #%li failed\n", tries);
		vga_console_puts(info);
		++tries;
	}
	snprintf(info, 100, "PS/2 first port passed test\n");
	vga_console_puts(info);

	probe_ret_t probe_result = __ps2_probe_devices();
	if (probe_result != probe_ok) {
		vga_console_puts("No PS/2 devices found!\n");
		return;
	}

	register_irq_handler(__PS2_FIRST_LINE_IRQ,
			handle_first_ps2_dev_int);
	register_irq_handler(__PS2_SECOND_LINE_IRQ,
			handle_second_ps2_dev_int);

	// enable interrupts
	uint8_t enable_int_cb = __ps2_read_conf_byte();
	enable_int_cb |= ps2_cb_int_first;
	__ps2_write_conf_byte(enable_int_cb);

	ps2_enable_dev(ps2_dev_1);
};

static void handle_first_ps2_dev_int(irq_t vec __attribute__((unused))) {
	__handle_ps2_interrupt(ps2_dev_1);
}

static void handle_second_ps2_dev_int(irq_t vec __attribute__((unused))) {
	__handle_ps2_interrupt(ps2_dev_2);
}

static void __handle_ps2_interrupt(ps2_line_t line) {
	if (ps2_active_devices[line]) {
		ps2_active_devices[line]->irq_handler();
	} else {
		char error_msg[50];
		snprintf(error_msg, 50, "No ps2 device exists for %li "
				"PS/2 line\n", (long) line);
		bug(error_msg);
	}
}

static uint8_t __ps2_read_conf_byte() {
	uint8_t conf_byte;
	outb(ps2_command_reg, ps2_cmd_read_cb);
	conf_byte = ps2_receive_byte();
	return conf_byte;
}

static void __ps2_write_conf_byte(uint8_t conf_byte) {
	outb(ps2_command_reg, ps2_cmd_write_cb);
	outb(ps2_data_port, conf_byte);
}

static probe_ret_t __ps2_probe_devices() {
	probe_ret_t global_probe_ret = probe_next;
	// for each line do ...
	for (ps2_line_t ps2_line = __ps2_dev_min;
			ps2_line < __ps2_dev_max; ++ps2_line) {
		struct ps2_dev *active_dev = NULL;
		int active_dev_count = 0;
		// for each registered device
		for (int dev_index = 0; dev_index < ps2_known_device_count;
				++dev_index) {
			probe_ret_t ret;
			ret = ps2_known_devices[dev_index]->probe(ps2_line);
			if (ret == probe_ok) {
				active_dev = ps2_known_devices[dev_index];
				active_dev_count++;
				global_probe_ret = probe_ok;
			}
		}
		if (active_dev_count > 1) {
			char msg[100];
			snprintf(msg, 100, "more than 1 probed devices for "
				"PS/2 line %li\n", (long) ps2_line);
			bug(msg);
		}
		ps2_active_devices[ps2_line] = active_dev;
	}
	return global_probe_ret;
}

