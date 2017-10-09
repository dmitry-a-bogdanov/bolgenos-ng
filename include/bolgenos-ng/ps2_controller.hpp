#pragma once

#include <cstddef>
#include <forward_list>

#include <bolgenos-ng/irq.hpp>

#include "ps2/common.hpp"

/**
* Timeout for self-test procedure.
*/
#define SELFTEST_TIMEOUT			5 /* ms */



namespace ps2 {

class IPS2Device;

/**
* \brief Register driver in PS/2 subsystem.
*
* Register PS/2 driver described by ps2_dev structure in PS/2 subsystem.
*
* \warning All PS/2 drivers should be registered in PS/2 subsystem before
*	calling this function.
*/
void register_device(IPS2Device *dev);

/**
* \brief Enum that holds PS/2 line number.
*
* Enum keeps PS/2 line for device. x86 family doesn't support more than two
*	PS/2 device, thus enum has only two values.
*/
enum line_t {
	dev_1		= 0x0, /*!< First PS/2 device. */
	dev_2		= 0x1, /*!< Second PS/2 device. */
	max_devs	= 2, /*!< Max number of PS/2 devices. */
};


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


// PS/2 status register bits
enum status_reg_t: uint8_t {
	out_buf_status		= 1 << 0,
	in_buf_status		= 1 << 1,
	post_passed		= 1 << 2,
	comm_or_data		= 1 << 3,
	// unknown		= 1 << 4,
	// unknown		= 1 << 5,
	ps2_status_timeout	= 1 << 6,
	parity_error		= 1 << 7
};


class IPS2Line;


class PS2Controller
{
public:
	PS2Controller(const PS2Controller& ) = delete;
	PS2Controller(PS2Controller&&) = delete;
	PS2Controller& operator=(const PS2Controller&) = delete;
	PS2Controller& operator=(PS2Controller&&) = delete;

	~PS2Controller();


	static PS2Controller *instance();

	IPS2Line* get_line(ps2::line_t line);


	/// \brief Initialize PS/2 subsystem.
	///
	/// Initialize PS/2 subsystem. Self-test controller and devices. Probe all
	///	all registered devices and setup drivers if there are successfully
	///	probed devices.
	///
	/// \warning All PS/2 drivers should be registered in PS/2 subsystem before
	///	calling this function.
	void initialize_controller();

	template<typename Func>
	void for_each_line(Func f) {
		IPS2Line *line;
		for (int line_id = 0;
				(line = get_line(static_cast<ps2::line_t>(line_id))) != nullptr;
				++line_id)
			f(line);
	}


	/// \brief Check that read from PS/2 is possible.
	///
	/// Function check that reading from data register of PS/2 controller is
	///	is possible without blocking.
	/// \return true if reading without blocking is possible.
	bool can_read();


	/// \brief Read byte from PS/2.
	///
	/// Read one byte fro PS/2 data port.
	/// \return Byte that was read.
	uint8_t receive_byte();

	bool receive(int timeout_ms, uint8_t *data);


	/// \brief Read PS/2 status.
	///
	/// The function reads PS/2 status register.
	///
	/// \return PS/2 status register.
	uint8_t status();

	void clean_buffer();


	/// \brief Read configuration byte.
	///
	/// The function reads configuration byte from PS/2 controller
	///
	/// \return Configuration byte.

	Conf conf();



	/// \brief Write configuration byte.
	///
	/// The function writes configuration byte to PS/2 controller.
	///
	/// \param conf_byte Configuration byte to be written.
	void conf(Conf conf);

	/**
	* \brief Send byte.
	*
	* Send byte to data port.
	*
	* \param byte Byte to be sent.
	*/
	void send_byte(uint8_t byte);

	void send_command(ps2::cmd_t cmd);


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
	bool wait_for_flag(status_reg_t flag, bool val, int ms);


	/**
	* \brief Wait for ability to read from PS/2.
	*
	* Function waits until it will be possible to read from PS/2 controller.
	* \param ms Time to wait in milliseconds.
	* \return 1 if reading to PS/2 is possible; 0 otherwice.
	*/
	bool wait_for_input(int ms = 0);




	/**
	* \brief Wait for ability to write to PS/2.
	*
	* Function waits until it will be possible to write to PS/2 controller.
	* \param ms Time to wait in milliseconds.
	* \return 1 if writing to PS/2 is possible; 0 otherwise.
	*/
	bool wait_for_output(int ms);


	/// \brief Run PS/2 controller self-test.
	///
	/// The function runs PS/2 controller self-test with \ref SELFTEST_TIMEOUT and
	/// returns status.
	bool do_selftest();


	/// \brief Handle IRQ from PS/2 device.
	///
	/// The function transfers handling of interrupt to corresponding driver.
	///
	/// \param line PS/2 device that raised interrupt.
	irq::IRQHandler::status_t ps2_irq_handler(ps2::line_t line);

	void register_driver(IPS2Device *dev);
private:
	PS2Controller();
	void probe_line(IPS2Line* line);
	void probe_devices();


	ps2::IPS2Device *ps2_active_devices_[ps2::line_t::max_devs];
	std::forward_list<ps2::IPS2Device*> _known_devices{};
}; // class PS2Controller


} // namespace ps2
