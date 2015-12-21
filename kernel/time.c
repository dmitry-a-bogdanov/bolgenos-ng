#include <bolgenos-ng/time.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/pit.h>

#include "config.h"

volatile uint32_t jiffies = 0;


void __sleep(uint32_t ticks_timeout) {
	uint32_t end_of_sleep = jiffies + ticks_timeout;
	while (jiffies < end_of_sleep) {
		halt_cpu();
	}
}


void sleep_ms(uint32_t ms) {
	__sleep(ms_to_ticks(ms));
}
