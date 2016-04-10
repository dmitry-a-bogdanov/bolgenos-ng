#include <bolgenos-ng/pit.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/time.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/pic_common.hpp>
#include <bolgenos-ng/stdtypes.hpp>

#include "frequency_divider.hpp"

#include "config.h"

void test_backtrace3(int arg);

void test_backtrace2(int arg) {
	cio::cout << __func__ << ":" << reinterpret_cast<void *>(test_backtrace2) << cio::endl;
	test_backtrace3(arg);
}


namespace {


/// \brief PIT frequency.
///
/// Frequency of PIT chip 8253/8254. This frequency will be divided
/// by configured value.
using pit_freq = lib::integral_constant<unsigned long, 1193182>;


/// \brief Max divider.
///
/// Maximal number that can be used as frequency divider.
using max_divider = lib::integral_constant<unsigned long, 65535>;


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


pit::details::FrequencyDivider freq_divider;


/// \brief Handle timer interrupt.
///
/// The function is a timer interrupt handler.
static irq::irq_return_t handle_pit_irq(irq::irq_t) {
	if (freq_divider.do_tick()) {
#if VERBOSE_TIMER_INTERRUPT
		cio::cout << "jiffy #" << jiffies << cio::endl;
#endif
		++jiffies;
	}
	return irq::irq_return_t::handled;
}


} // namespace




void pit::init() {
	const irq::irq_t timer_irq = pic::min_pic_irq() + 0;

	freq_divider.set_frequency(HZ, pit_freq::value, max_divider::value);
	if (freq_divider.is_low_frequency())
		cio::cwarn << "PIT: losing accuracy of timer" << cio::endl;

	irq::register_irq_handler(timer_irq, handle_pit_irq);

	uint8_t cmd = pit_channel::ch0|acc_mode::latch|oper_mode::m2|num_mode::bin;

	outb(pit_port::cmd, cmd);
	outb(pit_port::timer, bitmask(freq_divider.pit_timeout(), 0, 0xff));
	outb(pit_port::timer, bitmask(freq_divider.pit_timeout(), 8, 0xff));
}

