#include <bolgenos-ng/pit.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/time.h>

#include "config.h"


/**
* \brief PIT frequency.
*
* Frequency of PIT chip 8253/8254. This frequency will be devided
* by configured value.
*/
#define PIT_FREQ			((unsigned long) 1193182)


/**
* \brief Max devider.
*
* Maximal number that can be used as frequency divider.
*/
#define MAX_DIVIDER			((unsigned long) 65535)


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


typedef struct {
	uint16_t pit;
	unsigned long counter;
} div_pair_t;


void calculate_divider(div_pair_t *div, const unsigned long hz) {
	unsigned long full_div = PIT_FREQ / hz;
	if (full_div == 0) {
		full_div = 1; // PIT interprets 0 as 0xffff
	}
	if (full_div > MAX_DIVIDER) {
		div->pit = 100;
		div->counter = full_div / 100;
	} else {
		div->pit = bitmask(full_div, 0, 0xffff);
		div->counter = 0;
	}
}


static div_pair_t div_restart;
static div_pair_t div_current;

/**
* \brief Handle timer interrupt.
*
* The function is a timer interrupt handler.
*/
static void handle_pit_irq(irq_t vector __attribute__((unused))) {
	if (div_current.counter != 0) {
		--div_current.counter;
		return;
	}
#if VERBOSE_TIMER_INTERRUPT
	printk("jiffy #%lu\n", jiffies);
#endif
	jiffies++;
	div_current = div_restart;
}

/**
* Timer IRQ line.
*/
#define __TIMER_IRQ	(min_pic_irq + 0)

void init_pit() {
	calculate_divider(&div_restart, HZ);
	if (div_restart.counter != 0)
		printk("[PIT]:WARNING: losing accuracy of timer\n");

	register_irq_handler(__TIMER_IRQ, handle_pit_irq);

	int cmd = channel_0|acc_mode_latch|oper_mode_2|num_mode_bin;

	outb(cmd_port, cmd);
	outb(timer_port, bitmask(div_restart.pit, 0, 0xff));
	outb(timer_port, bitmask(div_restart.pit, 8, 0xff));
}

uint32_t ms_to_ticks(uint32_t ms) {
	if (div_restart.counter == 0) {
		unsigned long freq = PIT_FREQ / div_restart.pit;
		return freq / 1000 * ms;
	} else {
		unsigned long freq =
			PIT_FREQ / (div_restart.pit * div_restart.counter);
		return freq / 1000 * ms;
	}
}
