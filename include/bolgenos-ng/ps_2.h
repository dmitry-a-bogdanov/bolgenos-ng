#ifndef __BOLGENOS_NG__PS_2_H__
#define __BOLGENOS_NG__PS_2_H__ (1)

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/device.h>

typedef enum {
	ps2_dev_1		= 0x0,
#define __ps2_dev_min		ps2_dev_1
	ps2_dev_2		= 0x1,
#define __ps2_dev_max		(ps2_dev_2 + 1)
} ps2_line_t;

// ps2_dev structure describes ps2 device driver and contains pointer to
// functions for handling device.
//
// Fields:
// @probe - function that checks that PS/2 device can be handled by the
//	driver that provides this ps2_dev structure.
// @irq_handler - function that will be called when getting IRQ from the device
//	managed by this driver.
struct ps2_dev {
	probe_ret_t (*probe)(ps2_line_t);
	void (*irq_handler)(void);
};

int ps2_wait_for_output(int retries, int tick_timeout);
int ps2_wait_for_input(int retries, int tick_timeout);
uint8_t ps2_receive_byte();

void ps2_clean_buffer();

typedef enum {
	ps2_ioret_ok = 0,
	ps2_ioret_wrong_ack,
	ps2_ioret_timeout,
	__ps2_ioret_max_error
} ps2_ioret_t;

char *ps2_ioret_strerror(ps2_ioret_t error);

int ps2_send_byte_dev(ps2_line_t line, uint8_t byte);
ps2_ioret_t ps2_send_byte_with_ack(ps2_line_t line, uint8_t byte, uint8_t ack);

void ps2_init();

void ps2_register_device(struct ps2_dev *dev);

#endif // __BOLGENOS_NG__PS_2_H__
