#include <bolgenos-ng/pit.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/int_types.h>

#include "config.h"


/**
* \brief PIT frequency.
*
* Frequency of PIT chip 8253/8254. This frequency will be devided
* by configured value.
*/
#define PIT_FREQ			(1193182)


/**
* \brief Max devider.
*
* Maximal number that can be used as frequency divider.
*/
#define MAX_DIVIDER			(65535)


typedef enum {
	timer_port			= 0x40,
	reserved_port			= 0x41,
	speaker_port			= 0x42,
	cmd_port			= 0x43,
} pic_port_t;

typedef enum {
	num_mode_bcd			= 0 << 0,
	num_mode_bin			= 1 << 0,
} num_mod_t;

typedef enum {
	oper_mode_0			= 0 << 1,
	oper_mode_1			= 1 << 1,
	oper_mode_2			= 2 << 1,
	oper_mode_3			= 3 << 1,
	oper_mode_4			= 4 << 1,
	oper_mode_5			= 5 << 1,
} oper_mode_t;

typedef enum {
	acc_mode_latch			= 0 << 4,
	acc_mode_low			= 1 << 4,
	acc_mode_high			= 2 << 4,
	acc_mode_both			= 3 << 4,
} acc_mode_t;

typedef enum {
	channel_0			= 0 << 6,
	channel_1			= 1 << 6,
	channel_2			= 2 << 6,
	read_back			= 3 << 6,
} channel_bits_t;

void init_pit() {
	unsigned long __divider = PIT_FREQ / HZ;
	if (__divider > (unsigned long) MAX_DIVIDER) {
		panic("Requested frequency is to low");
	}

	uint16_t divider = __divider;

	int cmd = channel_0|acc_mode_latch|oper_mode_2|num_mode_bin;

	outb(cmd_port, cmd);
	outb(timer_port, divider & 0xff);
	outb(timer_port, (divider >> 8) & 0xff);
}
