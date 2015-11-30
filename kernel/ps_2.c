#include <bolgenos-ng/ps_2.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>


void ps2_init() {
	char info[100];
	snprintf(info, 100, "initializing PS/2 controller...\n");
	vga_console_puts(info);
	outb(ps2_command_reg, ps2_cmd_disable_2);
	outb(ps2_command_reg, ps2_cmd_disable_1);

	//FIXME: flush chip's output buffer

	outb(ps2_command_reg, ps2_cmd_read_cb);
	uint8_t configuration_byte = inb(ps2_data_port);

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

	outb(ps2_command_reg, ps2_cmd_write_cb);
	outb(ps2_data_port, disable_int_cb);

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
	// FIXME: handle failure
	while (inb(ps2_data_port) != ps2_rpl_port_test_ok) {
		snprintf(info, 100, "try #%li failed\n", tries);
		vga_console_puts(info);
		++tries;
	}
	snprintf(info, 100, "PS/2 first port passed test\n");
	vga_console_puts(info);

	// enable device
	outb(ps2_command_reg, ps2_cmd_enable_1);

	// enable interrupts
	outb(ps2_command_reg, ps2_cmd_read_cb);
	uint8_t enable_int_cb = inb(ps2_data_port);
	enable_int_cb |= ps2_cb_int_first;
	outb(ps2_command_reg, ps2_cmd_write_cb);
	outb(ps2_data_port, enable_int_cb);
};


void handle_first_ps2_dev_int() {
	uint8_t byte = inb(ps2_data_port);
	char info[100];
	snprintf(info, 100, "got '%lu' from first PS/2\n",
		(long unsigned) byte);
	vga_console_puts(info);
}

void handle_second_ps2_dev_int() {
	uint8_t byte = inb(ps2_data_port);
	char info[100];
	snprintf(info, 100, "got '%lu' from second PS/2\n",
		(long unsigned) byte);
	vga_console_puts(info);
}
