#include <bolgenos-ng/pit.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/time.h>

#include <bolgenos-ng/cout.hpp>

#include "config.h"

namespace {

/**
* \brief PIT frequency.
*
* Frequency of PIT chip 8253/8254. This frequency will be devided
* by configured value.
*/
const unsigned long PIT_FREQ = 1193182;


/**
* \brief Max devider.
*
* Maximal number that can be used as frequency divider.
*/
const unsigned long MAX_DIVIDER = 65535;


enum pit_port: uint16_t {
	timer			= 0x40,
	speaker			= 0x42,
	cmd			= 0x43,
};


enum num_mode: uint8_t {
	bcd			= 0 << 0,
	bin			= 1 << 0,
};


enum oper_mode: uint8_t {
	m0			= 0 << 1,
	m1			= 1 << 1,
	m2			= 2 << 1,
	m3			= 3 << 1,
	m4			= 4 << 1,
	m5			= 5 << 1,
};


enum acc_mode: uint8_t {
	latch			= 0 << 4,
	low			= 1 << 4,
	high			= 2 << 4,
	both			= 3 << 4,
};


enum pit_channel: uint8_t {
	ch0			= 0 << 6,
	ch1			= 1 << 6,
	ch2			= 2 << 6,
	back			= 3 << 6,
};

class FrequencyDivider {
public:
	FrequencyDivider();
	FrequencyDivider(const FrequencyDivider&) = delete;
	FrequencyDivider& operator=(const FrequencyDivider&) = delete;

	~FrequencyDivider();

	void set_frequency(unsigned long hz);
	uint16_t pit_timeout() const;
	bool do_tick();
	bool is_low_frequency() const;
private:
	uint16_t pit_;
	unsigned long counter_;
	unsigned long restart_;
};





FrequencyDivider freq_divider;

/**
* \brief Handle timer interrupt.
*
* The function is a timer interrupt handler.
*/
static void handle_pit_irq(irq_t vector __attribute__((unused))) {
	if (!freq_divider.do_tick()) {
		return;
	}
#if VERBOSE_TIMER_INTERRUPT
	cio::cout << "jiffy #" << jiffies << cio::endl;
#endif
	++jiffies;
}

} // namespace


FrequencyDivider::FrequencyDivider()
		: pit_(0x0), counter_(0), restart_(0) {
}

FrequencyDivider::~FrequencyDivider() {
}


bool FrequencyDivider::do_tick() {
	if (counter_ != 0) {
		--counter_;
		return false;
	} else {
		counter_ = restart_;
		return true;
	}
}


uint16_t FrequencyDivider::pit_timeout() const {
	return pit_;
}


bool FrequencyDivider::is_low_frequency() const {
	return restart_ != 0;
}


void FrequencyDivider::set_frequency(unsigned long hz) {
	unsigned long full_div = PIT_FREQ / hz;
	if (full_div == 0) {
		full_div = 1; // PIT interprets 0 as 0xffff
	}
	if (full_div > MAX_DIVIDER) {
		pit_ = 100;
		restart_ = full_div / 100;
	} else {
		pit_ = bitmask(full_div, 0, 0xffff);
		restart_ = 0;
	}
	counter_ = restart_;
}


/**
* Timer IRQ line.
*/
#define __TIMER_IRQ	(min_pic_irq + 0)

void pit::init() {
	freq_divider.set_frequency(HZ);
	if (freq_divider.is_low_frequency())
		cio::cwarn << "PIT: losing accuracy of timer" << cio::endl;

	register_irq_handler(__TIMER_IRQ, handle_pit_irq);

	uint8_t cmd = pit_channel::ch0|acc_mode::latch|oper_mode::m2|num_mode::bin;

	outb(pit_port::cmd, cmd);
	outb(pit_port::timer, bitmask(freq_divider.pit_timeout(), 0, 0xff));
	outb(pit_port::timer, bitmask(freq_divider.pit_timeout(), 8, 0xff));
}

