#include <bolgenos-ng/ps_2.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>

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
static void __handle_ps2_interrupt(enum ps2_dev_idx line);

void ps2_enable_dev(enum ps2_dev_idx idx) {
	(void) idx;
}

void ps2_disable_dev(enum ps2_dev_idx idx) {
	enum ps2_command cmd;
	if (idx == ps2_dev_1) {
		cmd = ps2_cmd_disable_1;
	} else {
		cmd = ps2_cmd_disable_2;
	}
	outb(ps2_command_reg, cmd);
}



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
	outb(ps2_command_reg, ps2_cmd_self_test);
	long tries = 0;
	// FIXME: handle self-test failure
	while (inb(ps2_data_port) != ps2_rpl_self_test_ok) {
		snprintf(info, 100, "try #%li failed\n", tries);
		vga_console_puts(info);
		++tries;
	}
	snprintf(info, 100, "PS/2 controller passed self-test\n");
	vga_console_puts(info);

	// test PS/2 ports
	outb(ps2_command_reg, ps2_cmd_port_test_1);

	tries = 0;
	// FIXME handle failure
	// FIXME since this code related to PS/2 lines separately it's neeeded
	//	to move this code to probe function
	while (inb(ps2_data_port) != ps2_rpl_port_test_ok) {
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

	// enable interrupts
	uint8_t enable_int_cb = __ps2_read_conf_byte();
	enable_int_cb |= ps2_cb_int_first;
	__ps2_write_conf_byte(enable_int_cb);
	// enable device
	outb(ps2_command_reg, ps2_cmd_enable_1);

};

void handle_first_ps2_dev_int() {
	__handle_ps2_interrupt(ps2_dev_1);
}

void handle_second_ps2_dev_int() {
	__handle_ps2_interrupt(ps2_dev_2);
}

static void __handle_ps2_interrupt(enum ps2_dev_idx line) {
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
	conf_byte = inb(ps2_data_port);
	return conf_byte;
}

static void __ps2_write_conf_byte(uint8_t conf_byte) {
	outb(ps2_command_reg, ps2_cmd_write_cb);
	outb(ps2_data_port, conf_byte);
}

static probe_ret_t __ps2_probe_devices() {
	probe_ret_t global_probe_ret = probe_next;
	for (enum ps2_dev_idx ps2_line = __ps2_dev_min;
			ps2_line < __ps2_dev_max; ++ps2_line) {
		struct ps2_dev *active_dev = NULL;
		int active_dev_count = 0;
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

