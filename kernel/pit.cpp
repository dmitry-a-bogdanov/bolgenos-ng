#include <bolgenos-ng/pit.hpp>

#include <cstdint>
#include <memory>
#include <ostream>

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/init_queue.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/time.hpp>

#include "frequency_divider.hpp"


namespace {


/// \brief PIT frequency.
///
/// Frequency of PIT chip 8253/8254. This frequency will be divided
/// by configured value.
constexpr unsigned long PIT_FREQUENCY = 1193182;


/// \brief Max divider.
///
/// Maximal number that can be used as frequency divider.
constexpr unsigned long MAX_DIVIDER = 65535;


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


class PitIRQHandler: public irq::IRQHandler {
public:
	PitIRQHandler() = delete;
	PitIRQHandler(const PitIRQHandler&) = delete;
	PitIRQHandler(PitIRQHandler&&) = delete;
	PitIRQHandler& operator =(const PitIRQHandler&) = delete;
	PitIRQHandler& operator =(PitIRQHandler&&) = delete;


	explicit PitIRQHandler(pit::details::FrequencyDivider* divider)
			:_divider(std::move(divider))
	{
	}

	virtual ~PitIRQHandler() {}

	status_t handle_irq(irq::irq_t vector __attribute__((unused))) override
	{
		if (_divider->do_tick()) {
			if constexpr(VERBOSE_TIMER_INTERRUPT)
			{
				std::cout << "jiffy #" << bolgenos::jiffies << std::endl;
			}
			++bolgenos::jiffies;
		}
		return status_t::HANDLED;
	}

private:
	pit::details::FrequencyDivider *_divider;
	static constexpr bool VERBOSE_TIMER_INTERRUPT = false;
};


pit::details::FrequencyDivider freq_divider;
auto empty_deleter = [](PitIRQHandler *) {};
PitIRQHandler handler(&freq_divider);


} // namespace



void pit::init() {
	static bool initialized{ false };
	if (!initialized)
	{
		const irq::irq_t timer_irq = devices::InterruptController::instance()->min_irq_vector() + 0;

		freq_divider.set_frequency(bolgenos::HZ, PIT_FREQUENCY, MAX_DIVIDER);
		if (freq_divider.is_low_frequency())
			LOG_WARNING("PIT: losing accuracy of timer");

		uint8_t cmd = pit_channel::ch0|acc_mode::latch|oper_mode::m2|num_mode::bin;

		outb(pit_port::cmd, cmd);
		outb(pit_port::timer, bitmask(freq_divider.pit_timeout(), 0, uint8_t{0xff}));
		outb(pit_port::timer, bitmask(freq_divider.pit_timeout(), 8, uint8_t{0xff}));

		irq::InterruptsManager::instance()->add_handler(timer_irq, &handler);
	}
}


namespace
{

using namespace bolgenos::init;


FunctorRegisterer reg(prio_t::cpu_configuration, "Programmable Interval Timer",
		[]() -> bool
		{
			pit::init();
			return true;
		});

}
