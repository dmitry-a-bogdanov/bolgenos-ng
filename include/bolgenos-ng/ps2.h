#ifndef __BOLGENOS_NG__PS_2_H__
#define __BOLGENOS_NG__PS_2_H__

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/device.h>


/**
* \brief Enum that holds PS/2 line number.
*
* Enum keeps PS/2 line for device. x86 family doesn't support more than two
*	PS/2 device, thus enum has only two values.
*/
typedef enum {
	ps2_dev_1		= 0x0, /*!< First PS/2 device. */
	ps2_dev_2		= 0x1, /*!< Second PS/2 device. */
} ps2_line_t;


/**
* \brief First PS/2 dev index.
*
* First PS/2 dev index that can be used as start value for looping through
*	all PS/2 devices.
*/
#define __ps2_dev_min		ps2_dev_1


/**
* \brief First invalid PS/2 device index.
*
* The value following the last valid PS/2 device index. This constant is to
*	be used as upper bound for looping through all PS/2 devices.
*/
#define __ps2_dev_max		(ps2_dev_2 + 1)

/**
* \brief Descriptor for PS/2 device.
*
* ps_2dev structure describes ps2 device driver and contains pointer to
* functions for handling device.
*
*/
struct ps2_dev {
	probe_ret_t (*probe)(ps2_line_t); /*!< function that checks that
			* PS/2 device can be handled by the driver that
			* provides this ps2_dev structure.
			*/
	void (*irq_handler)(void); /*!< function that will be called when
			* receiving IRQ from the device managed by this driver.
			*/
};


/**
* \brief Wait for ability to write to PS/2.
*
* Function waits until it will be possible to write to PS/2 controller.
* \param ms Time to wait in milliseconds.
* \return 1 if writing to PS/2 is possible; 0 otherwice.
*/
int ps2_wait_for_output(int ms);


/**
* \brief Wait for ability to read from PS/2.
*
* Function waits until it will be possible to read from PS/2 controller.
* \param ms Time to wait in milliseconds.
* \return 1 if reading to PS/2 is possible; 0 otherwice.
*/
int ps2_wait_for_input(int ms);


/**
* \brief Check that read from PS/2 is possible.
*
* Function check that reading from data register of PS/2 controller is
*	is possible without blocking.
* \return 0 if non-blocking reading is impossible and non-zero value otherwise.
*/
int ps2_can_read();


/**
* \brief Read byte from PS/2.
*
* Read one byte fro PS/2 data port.
* \return Byte that was read.
*/
uint8_t ps2_receive_byte();


/**
* \brief Clean PS/2 output buffer.
*
* Drop all data that is stored in PS/2 controller output buffer.
*/
void ps2_clean_buffer();


/**
* \brief status of PS/2 IO operation.
*
* This enum holds status of PS/2 IO operations that are not guarantied to be
*	successfully performed.
*/
typedef enum {
	ps2_ioret_ok = 0, /*!< No erros detected. */
	ps2_ioret_wrong_ack, /*!< Wrong ack is received. */
	ps2_ioret_timeout, /*!< Operation was not finished in time. */
	ps2_ioret_unknown_error /*!< Unknown error. */
} ps2_ioret_t;


/**
* \brief Get text description for PS/2 IO operation status.
*
* This is strerror-like function that recognizes only PS/2 IO operations
*	errors.
* \param error Status of operation.
* \return Pointer to description string.
*/
char *ps2_ioret_strerror(ps2_ioret_t error);


/**
* \brief Send byte to PS/2 device instead of controller.
*
* Functions sends byte to specified PS/2 device instead of PS/2 controller.
* \param line PS/2 device index.
* \param byte Byte to be written.
* \return Status of PS/2 IO operation.
*/
ps2_ioret_t ps2_send_byte_dev(ps2_line_t line, uint8_t byte);


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
ps2_ioret_t ps2_send_byte_with_ack(ps2_line_t line, uint8_t byte, uint8_t ack);


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
void ps2_init();


/**
* \brief Register driver in PS/2 subsystem.
*
* Register PS/2 driver descrived by ps2_dev structure in PS/2 subsystem.
*
* \warning All PS/2 drivers should be registered in PS/2 subsystem before
*	calling this function.
*/
void ps2_register_device(struct ps2_dev *dev);


#endif // __BOLGENOS_NG__PS_2_H__
