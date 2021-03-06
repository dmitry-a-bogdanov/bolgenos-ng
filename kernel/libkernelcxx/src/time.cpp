#include <bolgenos-ng/time.hpp>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.hpp>
#include <sched.hpp>

#include "config.h"

lib::atomic<uint32_t> jiffies{0};


void sleep_(uint32_t ticks_timeout) {
	if (!irq::is_enabled()) {
		panic("sleep with disabled interrupts");
	}
	uint32_t end_of_sleep = jiffies.load() + ticks_timeout;
	while (jiffies.load() < end_of_sleep) {
		sched::yield();
	}
}


void sleep_ms(uint32_t ms) {
	sleep_(ms_to_ticks(ms));
}


#define MS_IN_SEC ((uint32_t) 1000)


inline static uint32_t _low_part(uint32_t val) {
	return val % MS_IN_SEC;
}


inline static uint32_t _high_part(uint32_t val) {
	return val / MS_IN_SEC;
}


uint32_t ms_to_ticks(uint32_t ms) {
	uint32_t ticks = 0;
	ticks += _high_part(ms) * _high_part(HZ) * 1000;
	ticks += _high_part(ms) * _low_part(HZ);
	ticks += _low_part(ms) * _high_part(HZ);
	uint32_t trunkated_part = _low_part(ms) * _low_part(HZ);
	ticks += trunkated_part / 1000;
	if (trunkated_part % 1000) {
		ticks += 1;
	}
	return ticks;
}
