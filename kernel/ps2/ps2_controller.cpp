#include <bolgenos-ng/ps2_controller.hpp>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/ps2_line.hpp>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.hpp>

#include <bolgenos-ng/ps2/device.hpp>

#include <lib/ostream.hpp>

#include "ps2_keyboard.hpp"

namespace {


enum ps2_port_t: uint16_t  {
	PS2_PORT_DATA		= 0x60,
	PS2_PORT_STATUS		= 0x64,
	PS2_PORT_COMMAND	= 0x64
};


/// \brief Get number of PS/2 lines.
///
/// The function calculates number of PS/2 lines from configuration bytes.
///
/// \param conf_byte Configuration byte.
/// \return Number of PS/2 lines.
int get_ps2_lines(ps2::Conf conf) {
	if (conf.is_set(ps2::Conf::bits::clock_second)) {
		return 2;
	} else {
		return 1;
	}
}


} // namespace

void ps2::PS2Controller::register_driver(ps2::IPS2Device *dev) {
	if (ps2_known_device_count_ < MAX_REGISTERED_DEVICES) {
		ps2_known_devices_[ps2_known_device_count_++] = dev;
	}
}

void init_subsystems() {
	ps2::keyboard::init();
}



/// IRQ for the first PS/2 line.
inline int FIRST_LINE_IRQ()
{
	return devices::InterruptController::instance()->min_irq_vector() + 1;
}


/// IRQ for the second PS/2 line.
inline int SECOND_LINE_IRQ()
{
	return devices::InterruptController::instance()->min_irq_vector() + 12;
}



ps2::PS2Controller::PS2Controller() {}

ps2::PS2Controller::~PS2Controller() {}

ps2::PS2Controller* ps2::PS2Controller::instance()
{
	static PS2Controller controller;
	return &controller;
}



template<typename T, size_t N>
constexpr size_t arr_size(T (&)[N])
{
	return N;
}


ps2::IPS2Line* ps2::PS2Controller::get_line(ps2::line_t line)
{
	static IPS2Line __lines[] = {
		{this,
			{
				/*.cmd_enable = */		ps2::cmd_t::enable_1,
				/*.cmd_disable = */		ps2::cmd_t::disable_1,
				/*.cmd_test = */		ps2::cmd_t::port_test_1,
				/*.need_redirection = */	false,
				/*.cmd_redirect = */		static_cast<ps2::cmd_t>(0),
				/*.enable_interrups_mask = */	ps2::Conf::bits::int_first,
				/*.id = */			0,
			}
		},
		{this,
			{
				/*.cmd_enable = */		ps2::cmd_t::enable_2,
				/*.cmd_disable = */		ps2::cmd_t::disable_2,
				/*.cmd_test = */		ps2::cmd_t::port_test_2,
				/*.need_redirection = */	true,
				/*.cmd_redirect = */		ps2::cmd_t::redirect_2nd,
				/*.enable_interrups_mask = */	ps2::Conf::bits::int_second,
				/*.id = */			1,
			}
		}
	};

	if (line < arr_size(__lines)) {
		return &__lines[line];
	} else {
		return nullptr;
	}
}

enum ps2_port_t: uint16_t  {
	data		= 0x60,
	status		= 0x64,
	command		= 0x64
};

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


void ps2::PS2Controller::probe_line(ps2::IPS2Line* line) {
	ps2::IPS2Device *active_dev = nullptr;
	int active_dev_count = 0;
	// for each registered device
	for (int dev_index = 0; dev_index < ps2_known_device_count_;
			++dev_index) {
		auto ret = ps2_known_devices_[dev_index]->probe(line);
		if (ret == probe_ok) {
			active_dev = ps2_known_devices_[dev_index];
			active_dev_count++;
		}
	}
	if (active_dev_count > 1) {
		char info[100];
		snprintf(info, 100, "more than 1 probed devices for "
			"PS/2 line %li\n", line);
		bug(info);
	}

	lib::cinfo << "PS/2[" << line->id() << "]: active_dev = " << active_dev << lib::endl;

	ps2_active_devices_[line->id()] = active_dev;
}


/// \brief Probe PS/2 devices.
///
/// Function probes all registered PS/2 drivers to all PS/2 devices.
void ps2::PS2Controller::probe_devices() {
	for_each_line([this](IPS2Line *line) { this->probe_line(line); });
}


irq::IRQHandler::status_t ps2::PS2Controller::ps2_irq_handler(ps2::line_t line) {
	if (ps2_active_devices_[line]) {
		return ps2_active_devices_[line]->handle_irq();
	}
	return irq::IRQHandler::status_t::NONE;
}


class PS2IRQHandler: public irq::IRQHandler {
public:
	status_t handle_irq(irq::irq_t vector) override
	{
		if (vector == FIRST_LINE_IRQ()) {
			return ps2::PS2Controller::instance()->ps2_irq_handler(ps2::line_t::dev_1);
		} else if (vector == SECOND_LINE_IRQ()) {
			return ps2::PS2Controller::instance()->ps2_irq_handler(ps2::line_t::dev_2);
		} else {
			return status_t::NONE;
		}
	}
};


void ps2::PS2Controller::initialize_controller() {
	init_subsystems();

	lib::cnotice << "PS/2: initializing controller..." << lib::endl;

	auto controller = PS2Controller::instance();

	auto first_line = controller->get_line(ps2::line_t::dev_1);
	auto second_line = controller->get_line(ps2::line_t::dev_2);

	for_each_line([](IPS2Line *line) { line->disable(); });

	controller->clean_buffer();

	auto conf = controller->conf();

	lib::cinfo << "PS/2: configuration byte = " << conf.get() << lib::endl;

	lib::cnotice << "PS/2: this system has " << get_ps2_lines(conf) << " port(s)" << lib::endl;

	for_each_line([&conf](IPS2Line *line) { conf.unset(line->enable_interrupts_mask()); });

	conf.unset(ps2::Conf::bits::translation);

	controller->conf(conf);

	if (!controller->do_selftest()) {
		panic("PS/2: controller self-test failed!");
	}

	second_line->enable();

	conf = controller->conf();

	second_line->disable();

	for_each_line([](ps2::IPS2Line *line) {
		if (!line->test()) {
			lib::cerr << "PS/2: line " << line->id()
				<< " failed self-test!" << lib::endl;
		} else {
			lib::cinfo << "PS/2: line " << line->id()
				<< " passed self-test" << lib::endl;
		}
	});


	auto irq_handler = new PS2IRQHandler();
	auto irq_manager = irq::InterruptsManager::instance();
	irq_manager->add_handler(FIRST_LINE_IRQ(), irq_handler);
	irq_manager->add_handler(SECOND_LINE_IRQ(), irq_handler);


	for_each_line([](IPS2Line *line) { line->enable(); });


	probe_devices();

	conf = controller->conf();

	if (ps2_active_devices_[line_t::dev_1]) {
		conf.set(first_line->enable_interrupts_mask());
	} else {
		first_line->disable();
	}

	if (ps2_active_devices_[line_t::dev_2]) {
		conf.set(second_line->enable_interrupts_mask());
	} else {
		second_line->disable();
	}

	controller->conf(conf);
}





uint8_t ps2::PS2Controller::status() {
	return inb(PS2_PORT_STATUS);
}


uint8_t ps2::PS2Controller::receive_byte()
{
	return inb(PS2_PORT_DATA);
}


bool ps2::PS2Controller::receive(int ms_timeout, uint8_t *data)
{
	if (wait_for_input(ms_timeout)) {
		*data = receive_byte();
		return true;
	} else {
		return false;
	}
}



bool ps2::PS2Controller::can_read()
{
	return wait_for_input();
}

void ps2::PS2Controller::clean_buffer()
{
	auto status_register = status();
	while (status_register & status_reg_t::out_buf_status) {
		this->receive_byte(); // ignore input
		status_register = status();
	}
}


ps2::Conf ps2::PS2Controller::conf()
{
	send_command(cmd_t::read_cb);
	return Conf(receive_byte());
}


void ps2::PS2Controller::conf(Conf conf)
{
	send_command(cmd_t::write_cb);
	send_byte(conf.get());
}


void ps2::PS2Controller::send_byte(uint8_t byte)
{
	outb(PS2_PORT_DATA, byte);
}

void ps2::PS2Controller::send_command(ps2::cmd_t cmd)
{
	outb(PS2_PORT_COMMAND, cmd);
}

bool ps2::PS2Controller::wait_for_flag(status_reg_t flag, bool val, int ms) {
	uint8_t st;
	int wait = 0;
	if (val) {
		while (!((st = status()) & flag)
				&& wait < ms) {
			++wait;
			sleep_ms(1 /* ms */);
		}
	} else {
		while (((st = status()) & flag)
				&& wait < ms) {
			++wait;
			sleep_ms(1 /* ms */);
		}
	}
	return st & flag;
}



bool ps2::PS2Controller::wait_for_input(int ms) {
	return wait_for_flag(status_reg_t::out_buf_status, true, ms);
}


/// \brief Wait for ability to write to PS/2.
///
/// Function waits until it will be possible to write to PS/2 controller.
/// \param ms Time to wait in milliseconds.
/// \return 1 if writing to PS/2 is possible; 0 otherwise.
bool ps2::PS2Controller::wait_for_output(int ms)
{
	return !wait_for_flag(status_reg_t::in_buf_status, false, ms);
}


bool ps2::PS2Controller::do_selftest()
{
	send_command(cmd_t::self_test);

	uint8_t test_result;
	if (!receive(SELFTEST_TIMEOUT, &test_result)) {
		return false; // self-test timed out
	}
	return test_result == test_reply::self_test_ok;
}
