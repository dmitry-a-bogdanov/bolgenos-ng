#pragma once

#include <bolgenos-ng/device.h>

#include <bolgenos-ng/stdtypes.hpp>

namespace ps2 {

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

template<typename Func>
void for_each_line(Func f) {
	for(int line = 0; line < line_t::max_devs; ++line) f(line_t(line));
};

/**
* \brief Descriptor for PS/2 device.
*
* ps_2dev structure describes ps2 device driver and contains pointer to
* functions for handling device.
*
*/
class ps2_dev {
public:
	virtual probe_ret_t probe(line_t) = 0; /*!< function that checks that
			* PS/2 device can be handled by the driver that
			* provides this ps2_dev structure.
			*/
	virtual void handle_irq() = 0; /*!< function that will be called when
			* receiving IRQ from the device managed by this driver.
			*/
	virtual ~ps2_dev() {}
};




/**
* \brief Wait for ability to read from PS/2.
*
* Function waits until it will be possible to read from PS/2 controller.
* \param ms Time to wait in milliseconds.
* \return 1 if reading to PS/2 is possible; 0 otherwice.
*/
int wait_for_input(int ms);


/**
* \brief Check that read from PS/2 is possible.
*
* Function check that reading from data register of PS/2 controller is
*	is possible without blocking.
* \return 0 if non-blocking reading is impossible and non-zero value otherwise.
*/
int can_read();


/**
* \brief Read byte from PS/2.
*
* Read one byte fro PS/2 data port.
* \return Byte that was read.
*/
uint8_t receive_byte();


/**
* \brief Clean PS/2 output buffer.
*
* Drop all data that is stored in PS/2 controller output buffer.
*/
void clean_buffer();


/**
* \brief status of PS/2 IO operation.
*
* This enum holds status of PS/2 IO operations that are not guarantied to be
*	successfully performed.
*/
enum ioret_t {
	ok		= 0, /*!< No erros detected. */
	wrong_ack	= 1, /*!< Wrong ack is received. */
	timeout		= 2, /*!< Operation was not finished in time. */
	unknown		= 3, /*!< Unknown error. */
};


/**
* \brief Get text description for PS/2 IO operation status.
*
* This is strerror-like function that recognizes only PS/2 IO operations
*	errors.
* \param error Status of operation.
* \return Pointer to description string.
*/
const char *strerror(ioret_t error);


/**
* \brief Send byte to PS/2 device and wait for ack.
*
* Function sends byte to PS/2 device instead of controller and waits for
*	for receiving ack.
* \param line PS/2 device index.
* \param byte Byte to be written.
* \param ack Byte that should be received as ack.
* \return Status of PS/2 IO operation.
*/
ioret_t send_byte_with_ack(line_t line, uint8_t byte, uint8_t ack);


/**
* \brief Initilize PS/2 subsystem.
*
* Initialize PS/2 subsystem. Self-test controller and devices. Probe all
*	all registered devices and setup drivers if there are successfully
*	probed devices.
*
* \warning All PS/2 drivers should be registered in PS/2 subsystem before
*	calling this function.
*/
void init();


/**
* \brief Register driver in PS/2 subsystem.
*
* Register PS/2 driver descrived by ps2_dev structure in PS/2 subsystem.
*
* \warning All PS/2 drivers should be registered in PS/2 subsystem before
*	calling this function.
*/
void register_device(struct ps2_dev *dev);

} // namespace ps2
