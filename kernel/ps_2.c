#include <bolgenos-ng/ps_2.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.h>
#include <bolgenos-ng/vga_console.h>

enum ps2_reply {
	ps2_rpl_port_test_ok		= 0x00,
	ps2_rpl_port_test_clk_low	= 0x01,
	ps2_rpl_port_test_clk_high	= 0x02,
	ps2_rpl_port_test_data_low	= 0x03,
	ps2_rpl_port_test_data_high	= 0x04,

	ps2_rpl_self_test_ok		= 0x55,
	ps2_rpl_self_test_fail		= 0xfc
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

static void ps2_send_byte(uint8_t byte) {
	outb(ps2_data_port, byte);
}

void ps2_send_command(ps2_command_t cmd) {
	outb(ps2_command_reg, cmd);
}

static uint8_t ps2_read_status() {
	return inb(ps2_status_reg);
}

static int ps2_wait_for_input(int retries, int tick_timeout) {
	uint8_t status;
	int tries = 0;
	while (!((status = ps2_read_status()) & ps2_sr_out_buf_status)
			&& tries < retries) {
		++tries;
		__sleep(tick_timeout);
	}
	return status & ps2_sr_out_buf_status;
}


#define MAX_PS2_KNOWN_DEVS		10
#define PS2_SELF_TEST_RETRIES		5
#define PS2_SELF_TEST_TIMEOUT		1 /* ticks */

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

static uint8_t ps2_read_conf_byte();
static void ps2_write_conf_byte(uint8_t conf_byte);

static void ps2_probe_devices();

static void ps2_irq_handler(ps2_line_t line);

static void first_line_isr(irq_t);
static void second_line_isr(irq_t);

static void ps2_enable_dev(ps2_line_t idx);
static void ps2_disable_dev(ps2_line_t idx);

static int ps2_get_lines(uint8_t conf_byte);

static void ps2_enable_interrupts(uint8_t *conf_byte, int lines);
static void ps2_disable_interrupts(uint8_t *conf_byte, int lines);

static void ps2_disable_translation(uint8_t *conf_byte);

static void ps2_clean_buffer();
static int ps2_controller_test();

static int ps2_test_line(ps2_line_t line);

#define PS2_FIRST_LINE_IRQ		(min_pic_irq + 1)
#define PS2_SECOND_LINE_IRQ		(min_pic_irq + 12)

void ps2_init() {
	char info[100];
	uint8_t conf;
	snprintf(info, 100, "initializing PS/2 controller...\n");
	vga_console_puts(info);
	ps2_disable_dev(ps2_dev_2);
	ps2_disable_dev(ps2_dev_1);

	ps2_clean_buffer();

	conf = ps2_read_conf_byte();

	snprintf(info, 100, "PS/2 conf byte=%lu\n",
		(unsigned long) conf);
	vga_console_puts(info);

	int ps2_lines = ps2_get_lines(conf);

	snprintf(info, 100, "this system has %lu PS/2 port(s)\n",
			(long unsigned) ps2_lines);
	vga_console_puts(info);

	ps2_disable_interrupts(&conf, ps2_dev_1|ps2_dev_1);
	ps2_disable_translation(&conf);

	ps2_write_conf_byte(conf);

	if (!ps2_controller_test()) {
		panic("PS/2: controller self-test failed!");
	}

	ps2_probe_devices();

	register_irq_handler(PS2_FIRST_LINE_IRQ, first_line_isr);
	register_irq_handler(PS2_SECOND_LINE_IRQ, second_line_isr);

	conf = ps2_read_conf_byte();
	ps2_enable_interrupts(&conf, ps2_dev_1);
	ps2_write_conf_byte(conf);

	ps2_enable_dev(ps2_dev_1);
};

static void first_line_isr(irq_t vec __attribute__((unused))) {
	ps2_irq_handler(ps2_dev_1);
}

static void second_line_isr(irq_t vec __attribute__((unused))) {
	ps2_irq_handler(ps2_dev_2);
}

static void ps2_irq_handler(ps2_line_t line) {
	if (ps2_active_devices[line]) {
		ps2_active_devices[line]->irq_handler();
	} else {
		char error_msg[50];
		snprintf(error_msg, 50, "No ps2 device exists for %li "
				"PS/2 line\n", (long) line);
		bug(error_msg);
	}
}

static uint8_t ps2_read_conf_byte() {
	uint8_t conf_byte;
	ps2_send_command(ps2_cmd_read_cb);
	conf_byte = ps2_receive_byte();
	return conf_byte;
}

static void ps2_write_conf_byte(uint8_t conf_byte) {
	ps2_send_command(ps2_cmd_write_cb);
	ps2_send_byte(conf_byte);
}

static void ps2_probe_devices() {
	char info[100];

	// for each line do ...
	for (ps2_line_t ps2_line = __ps2_dev_min;
			ps2_line < __ps2_dev_max; ++ps2_line) {
		if (!ps2_test_line(ps2_line)) {
			snprintf(info, 100, "PS/2: line %li"
					" failed self-test!\n",
					(long) ps2_line);
			vga_console_puts(info);
			continue;
		} else {
			snprintf(info, 100, "PS/2: line %li"
					" passed self-test\n",
					(long) ps2_line);
			vga_console_puts(info);
		}
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
			}
		}
		if (active_dev_count > 1) {
			snprintf(info, 100, "more than 1 probed devices for "
				"PS/2 line %li\n", (long) ps2_line);
			bug(info);
		}

		snprintf(info, 100, "PS/2[%li]: active_dev = %li\n",
			(long) ps2_line, (long) active_dev);
		vga_console_puts(info);

		ps2_active_devices[ps2_line] = active_dev;
	}
}

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

static int ps2_get_lines(uint8_t conf_byte) {
	int ps2_lines = 1;
	if (conf_byte & ps2_cb_clock_second) {
		ps2_lines = 2;
	}
	return ps2_lines;
}

static void ps2_enable_interrupts(uint8_t *conf_byte, int lines) {
	if (lines & ps2_dev_1) {
		*conf_byte |= ps2_cb_int_first;
	}
	if (lines & ps2_dev_2) {
		*conf_byte |= ps2_cb_int_second;
	}
}

static void ps2_disable_interrupts(uint8_t *conf_byte, int lines) {
	if (lines & ps2_dev_1) {
		*conf_byte &= ~ps2_cb_int_first;
	}
	if (lines & ps2_dev_2) {
		*conf_byte &= ~ps2_cb_int_second;
	}
}

static void ps2_disable_translation(uint8_t *conf_byte) {
	*conf_byte &= ~ps2_cb_translation;
}

static void ps2_clean_buffer() {
	uint8_t status_register = ps2_read_status();
	while (status_register & ps2_sr_out_buf_status) {
		ps2_receive_byte(); // ignore input
		status_register = ps2_read_status();
	}
}

static int ps2_controller_test() {
	ps2_send_command(ps2_cmd_self_test);
	int can_read = ps2_wait_for_input(PS2_SELF_TEST_RETRIES,
			PS2_SELF_TEST_TIMEOUT);
	if (!can_read) {
		// self-test timeout!
		return 0;
	}
	uint8_t test_result = ps2_receive_byte();
	if (test_result == ps2_rpl_self_test_ok) {
		return 1;
	}
	return 0;
}


static int ps2_test_line(ps2_line_t line) {
	ps2_command_t cmd;
	if (line == ps2_dev_1) {
		cmd = ps2_cmd_port_test_1;
	} else {
		cmd = ps2_cmd_port_test_2;
	}
	ps2_send_command(cmd);
	int can_read = ps2_wait_for_input(PS2_SELF_TEST_RETRIES,
			PS2_SELF_TEST_TIMEOUT);
	if (!can_read) {
		return 0;
	}
	uint8_t test_result = ps2_receive_byte();
	if (test_result == ps2_rpl_port_test_ok) {
		return 1;
	}
	return 0;
}
