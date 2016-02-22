#include <bolgenos-ng/ps2.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.h>


/**
* IRQ for the first PS/2 line.
*/
#define FIRST_LINE_IRQ		(min_pic_irq + 1)


/**
* IRQ for the second PS/2 line.
*/
#define SECOND_LINE_IRQ		(min_pic_irq + 12)


/**
* Maximal number of devices that can be registered.
*/
#define MAX_REGISTERED_DEVICES			10


/**
* Timeout for self-test procedure.
*/
#define SELFTEST_TIMEOUT			5 /* ms */


/**
* Timeout for output operation.
*/
#define OUTPUT_TIMEOUT				1 /* ms */


enum test_reply: uint8_t {
	port_test_ok		= 0x00,
	port_test_clk_low	= 0x01,
	port_test_clk_high	= 0x02,
	port_test_data_low	= 0x03,
	port_test_data_high	= 0x04,

	self_test_ok		= 0x55,
	self_test_fail		= 0xfc
};

enum cmd_t: uint8_t {
	read_cb		= 0x20,
	write_cb	= 0x60,

	self_test	= 0xaa,
	port_test_1	= 0xab,
	port_test_2	= 0xa9,

	disable_1	= 0xad,
	enable_1	= 0xae,
	disable_2	= 0xa7,
	enable_2	= 0xa8,

	redirect_2nd	= 0xd4
};

static void send_command(cmd_t cmd);


// PS/2 status register bits
enum status_reg_t: uint8_t {
	out_buf_status	= 1 << 0,
	in_buf_status	= 1 << 1,
	post_passed	= 1 << 2,
	comm_or_data	= 1 << 3,
	// unknown		= 1 << 4,
	// unknown		= 1 << 5,
	timeout		= 1 << 6,
	parity_error	= 1 << 7
};


// PS/2 configuration byte bits
enum conf_byte_t: uint8_t {
	int_first	= 1 << 0,
	int_second	= 1 << 1,
	system		= 1 << 2,
	clock_first	= 1 << 4,
	clock_second	= 1 << 5,
	translation	= 1 << 6
};




enum ps2_port_t: uint16_t  {
	data		= 0x60,
	status		= 0x64,
	command		= 0x64
};



// TODO: replace array with list in order to avoid troubles with support
// of many types of keyboards or mice
static ps2::ps2_dev *ps2_known_devices[MAX_REGISTERED_DEVICES];
static int ps2_known_device_count = 0;


static class ps2::ps2_dev *ps2_active_devices[ps2::line_t::__dev_max];

static void send_byte(uint8_t byte);
static ps2::ioret_t send_byte_dev(ps2::line_t line, uint8_t byte);
static uint8_t read_conf_byte();
static void write_conf_byte(uint8_t conf_byte);
static void probe_devices();
static void ps2_irq_handler(ps2::line_t line);
static void first_line_irq(irq_t);
static void second_line_irq(irq_t);
static void enable_device(ps2::line_t idx);
static void disable_device(ps2::line_t idx);
static int get_ps2_lines(uint8_t conf_byte);
static void enable_ps2_interrupts(uint8_t *conf_byte, int lines);
static void disable_ps2_interrupts(uint8_t *conf_byte, int lines);
static void disable_translation(uint8_t *conf_byte);
static int controller_selftest();
static int test_line(ps2::line_t line);
static uint8_t read_status();
static int wait_for_flag(status_reg_t flag, int val, int ms);
static int wait_for_output(int ms);

void ps2::register_device(ps2::ps2_dev *dev) {
	if (ps2_known_device_count < MAX_REGISTERED_DEVICES) {
		ps2_known_devices[ps2_known_device_count++] = dev;
	}
}


void ps2::init() {
	uint8_t conf;
	printk("initializing PS/2 controller...\n");
	disable_device(ps2::line_t::dev_1);
	disable_device(ps2::line_t::dev_2);

	clean_buffer();

	conf = read_conf_byte();

	printk("PS/2 conf byte=%lu\n", (unsigned long) conf);

	int ps2_lines = get_ps2_lines(conf);

	printk("this system has %lu PS/2 port(s)\n",
		(long unsigned) ps2_lines);

	disable_ps2_interrupts(&conf, line_t::dev_1|line_t::dev_2);
	disable_translation(&conf);

	write_conf_byte(conf);

	if (!controller_selftest()) {
		panic("PS/2: controller self-test failed!");
	}

	enable_device(line_t::dev_2);
	
	conf = read_conf_byte();
	if (conf & (conf_byte_t::clock_second|conf_byte_t::clock_first)) {
		printk("both PS/2 devs present\n");
	} else if (conf & conf_byte_t::clock_first) {
		printk("only first PS/2 dev present\n");
	} else if (conf & conf_byte_t::clock_second) {
		printk("only second PS/2 dev present\n");
	} else {
		printk("no devs present\n");
	}

	disable_device(line_t::dev_2);

	for (int ps2_line = line_t::__dev_min;
			ps2_line < line_t::__dev_max; ++ps2_line) {
		if (!test_line(line_t(ps2_line))) {
			printk("PS/2: line %li failed self-test!\n",
					(long) ps2_line);
		} else {
			printk("PS/2: line %li passed self-test\n",
					(long) ps2_line);
		}
	}

	register_irq_handler(FIRST_LINE_IRQ, first_line_irq);
	register_irq_handler(SECOND_LINE_IRQ, second_line_irq);


	enable_device(line_t::dev_1);
	enable_device(line_t::dev_2);

	probe_devices();

	conf = read_conf_byte();
	enable_ps2_interrupts(&conf, line_t::dev_1|line_t::dev_2);
	write_conf_byte(conf);
};


uint8_t ps2::receive_byte() {
	return inb(ps2_port_t::data);
}


void send_command(cmd_t cmd) {
	outb(ps2_port_t::command, cmd);
}


/**
* \brief Wait for ability to write to PS/2.
*
* Function waits until it will be possible to write to PS/2 controller.
* \param ms Time to wait in milliseconds.
* \return 1 if writing to PS/2 is possible; 0 otherwice.
*/
int wait_for_output(int ms) {
	return ! wait_for_flag(status_reg_t::in_buf_status, 0, ms);
}


int ps2::wait_for_input(int ms) {
	return !! wait_for_flag(status_reg_t::out_buf_status, 1, ms);
}


int ps2::can_read() {
	return wait_for_input(0);
}


void ps2::clean_buffer() {
	uint8_t status_reg_tister = read_status();
	while (status_reg_tister & status_reg_t::out_buf_status) {
		receive_byte(); // ignore input
		status_reg_tister = read_status();
	}
}


/**
* \brief Send byte to PS/2 device instead of controller.
*
* Functions sends byte to specified PS/2 device instead of PS/2 controller.
* \param line PS/2 device index.
* \param byte Byte to be written.
* \return Status of PS/2 IO operation.
*/
ps2::ioret_t send_byte_dev(ps2::line_t line, uint8_t byte) {
	if (line == ps2::line_t::dev_2) {
		send_command(cmd_t::redirect_2nd);
	}
	int can_write = wait_for_output(OUTPUT_TIMEOUT);
	if (!can_write) {
		return ps2::ioret_t::timeout;
	}
	send_byte(byte);
	return ps2::ioret_t::ok;
}


ps2::ioret_t ps2::send_byte_with_ack(ps2::line_t line, uint8_t byte,
		uint8_t ack) {
	ioret_t ret;
	ret = send_byte_dev(line, byte);
	if (ret != ioret_t::ok) {
		return ret;
	}
	if (wait_for_input(OUTPUT_TIMEOUT)) {
		uint8_t byte = receive_byte();
		if (byte == ack) {
			return ioret_t::ok;
		} else {
			return ioret_t::wrong_ack;
		}
	} else {
		return ioret_t::timeout;
	}
}


/**
* \brief Error descriptions.
*
* Array with strings that describes possible PS2 I/O errors.
*/
static const char *ps2_err_descr[] = {
	[ps2::ioret_t::ok] = "No error",
	[ps2::ioret_t::wrong_ack] = "Wrong ack",
	[ps2::ioret_t::timeout] = "Timeout",
	[ps2::ioret_t::unknown] = "Unknown error",
};

const char *ps2::strerror(ps2::ioret_t error) {
	if (error >= ps2::ioret_t::unknown)
		return ps2_err_descr[ps2::ioret_t::unknown];
	return ps2_err_descr[error];
}


/**
* \brief First PS/2 line interrupt handler.
*
* The function call \ref ps2_irq_handler with correct parameter.
*
* \param vec Unused parameter that is needed to match types.
*/
static void first_line_irq(irq_t vec __attribute__((unused))) {
	ps2_irq_handler(ps2::line_t::dev_1);
}


/**
* \brief Second PS/2 line interrupt handler.
*
* The function call \ref ps2_irq_handler with correct parameter.
*
* \param vec Unused parameter that is needed to match types.
*/
static void second_line_irq(irq_t vec __attribute__((unused))) {
	ps2_irq_handler(ps2::line_t::dev_2);
}


/**
* \brief Handle IRQ from PS/2 device.
*
* The function transfers handling of interrupt to corresponding driver.
*
* \param line PS/2 device that raised interrupt.
*/
static void ps2_irq_handler(ps2::line_t line) {
	if (ps2_active_devices[line]) {
		ps2_active_devices[line]->handle_irq();
	}
}


/**
* \brief Read configuration byte.
*
* The function reads configuration byte.
*
* \return Configuration byte.
*/
static uint8_t read_conf_byte() {
	uint8_t conf_byte;
	send_command(cmd_t::read_cb);
	conf_byte = ps2::receive_byte();
	return conf_byte;
}


/**
* \brief Write configuration byte.
*
* The function writes configuration byte to PS/2 controller.
*
* \param conf_byte Configuration byte to be written.
*/
static void write_conf_byte(uint8_t conf_byte) {
	send_command(cmd_t::write_cb);
	send_byte(conf_byte);
}


/**
* \brief Probe PS/2 devices.
*
* Function probes all registered PS/2 drivers to all PS/2 devices.
*/
static void probe_devices() {
	// for each line do ...
	for (int line = ps2::line_t::__dev_min;
			line < ps2::line_t::__dev_max; ++line) {
		ps2::ps2_dev *active_dev = nullptr;
		int active_dev_count = 0;
		// for each registered device
		for (int dev_index = 0; dev_index < ps2_known_device_count;
				++dev_index) {
			probe_ret_t ret;
			ret = ps2_known_devices[dev_index]->probe(ps2::line_t(line));
			if (ret == probe_ok) {
				active_dev = ps2_known_devices[dev_index];
				active_dev_count++;
			}
		}
		if (active_dev_count > 1) {
			char info[100];
			snprintf(info, 100, "more than 1 probed devices for "
				"PS/2 line %li\n", (long) line);
			bug(info);
		}

		printk("PS/2[%li]: active_dev = %li\n",
			(long) line, (long) active_dev);

		ps2_active_devices[line] = active_dev;
	}
}


/**
* \brief Enable PS/2 device.
*
* The function enables specified PS/2 device.
* \param idx Device to be enabled.
*/
static void enable_device(ps2::line_t idx) {
	if (idx == ps2::line_t::dev_1) {
		send_command(cmd_t::enable_1);
	} else {
		send_command(cmd_t::enable_2);
	}
}


/**
* \brief Disable PS/2 device.
*
* The function disables specified PS/2 device.
* \param idx Device to be disabled.
*/
static void disable_device(ps2::line_t idx) {
	cmd_t cmd;
	if (idx == ps2::line_t::dev_1) {
		cmd = cmd_t::disable_1;
	} else {
		cmd = cmd_t::disable_2;
	}
	send_command(cmd);
}


/**
* \brief Get number of PS/2 lines.
*
* The function calculates number of PS/2 lines from configuration bytes.
*
* \param conf_byte Configuration byte.
* \return Number of PS/2 lines.
*/
static int get_ps2_lines(uint8_t conf_byte) {
	int ps2_lines = 1;
	if (conf_byte & conf_byte_t::clock_second) {
		ps2_lines = 2;
	}
	return ps2_lines;
}


/**
* \brief Enable interrupts from devices.
*
* The function enable interrupts from specified PS/2 devices.
*
* \param conf_byte Pointer to configuration byte variable.
* \param lines Bitwise OR-ed PS/2 lines devices that should start raise
* interrupts.
*/
static void enable_ps2_interrupts(uint8_t *conf_byte, int lines) {
	if (lines & ps2::line_t::dev_1) {
		*conf_byte |= conf_byte_t::int_first;
	}
	if (lines & ps2::line_t::dev_2) {
		*conf_byte |= conf_byte_t::int_second;
	}
}


/**
* \brief Disable interrupts from devices.
*
* The function disables interrupts from specified PS/2 devices.
*
* \param conf_byte Pointer to configuration byte variable.
* \param lines Bitwise OR-ed PS/2 lines devices that should not raise
* interrupts.
*/
static void disable_ps2_interrupts(uint8_t *conf_byte, int lines) {
	if (lines & ps2::line_t::dev_1) {
		*conf_byte &= ~conf_byte_t::int_first;
	}
	if (lines & ps2::line_t::dev_2) {
		*conf_byte &= ~conf_byte_t::int_second;
	}
}


/**
* \brief Disable translation option.
*
* The function clears translation bytes in provided PS/2 configuration byte.
*
* \param conf_byte Pointer to configuration byte.
*/
static void disable_translation(uint8_t *conf_byte) {
	*conf_byte &= ~conf_byte_t::translation;
}


/**
* \brief Run PS/2 controller self-test.
*
* The function runs PS/2 controller self-test with \ref SELFTEST_TIMEOUT and
* returns status.
*/
static int controller_selftest() {
	send_command(cmd_t::self_test);
	int can_read = ps2::wait_for_input(SELFTEST_TIMEOUT);
	if (!can_read) {
		// self-test timeout!
		return 0;
	}
	uint8_t test_result = ps2::receive_byte();
	if (test_result == test_reply::self_test_ok) {
		return 1;
	}
	return 0;
}


/**
* \brief Send byte.
*
* Send byte to data port.
*
* \param byte Byte to be sent.
*/
static void send_byte(uint8_t byte) {
	outb(ps2_port_t::data, byte);
}


/**
* \brief Read PS/2 status.
*
* The function reads PS/2 status register.
*
* \return PS/2 status register.
*/
static uint8_t read_status() {
	return inb(ps2_port_t::status);
}


/**
* \brief Wait for flag in SR.
*
* The functions waits specified value for given flag druing specified time.
*
* \param flag Flag to wait.
* \param val Value for waiting.
* \param ms Time to wait.
* \return Value of flag after waiting.
*/
static int wait_for_flag(status_reg_t flag, int val, int ms) {
	uint8_t status;
	int wait = 0;
	if (val) {
		while (!((status = read_status()) & flag)
				&& wait < ms) {
			++wait;
			sleep_ms(1 /* ms */);
		}
	} else {
		while (((status = read_status()) & flag)
				&& wait < ms) {
			++wait;
			sleep_ms(1 /* ms */);
		}
	}
	return status & flag;
}


/**
* \brief Test PS/2 line.
*
* The function tests specified PS/2 line.
*
* \param line Line to be tested.
* \return 1 if test is ok; 0 otherwise.
*/
static int test_line(ps2::line_t line) {
	cmd_t cmd;
	if (line == ps2::line_t::dev_1) {
		cmd = cmd_t::port_test_1;
	} else {
		cmd = cmd_t::port_test_2;
	}
	send_command(cmd);
	int can_read = ps2::wait_for_input(SELFTEST_TIMEOUT);
	if (!can_read) {
		printk("no responce to self-test\n");
		return 0;
	}
	uint8_t test_result = ps2::receive_byte();
	if (test_result == test_reply::port_test_ok) {
		return 1;
	}
	printk("line test result = %lu\n", (long unsigned) test_result);
	return 0;
}
