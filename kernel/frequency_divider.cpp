#include "frequency_divider.hpp"

#include <bolgenos-ng/mem_utils.hpp>


pit::details::FrequencyDivider::~FrequencyDivider() {
}


uint16_t pit::details::FrequencyDivider::pit_timeout() const {
	return pit_;
}


bool pit::details::FrequencyDivider::is_low_frequency() const {
	return restart_ != 0;
}


void pit::details::FrequencyDivider::set_frequency(unsigned long hz,
		const unsigned long hw_pit_freq,
		const unsigned long hw_max_divider) {
	unsigned long full_div = hw_pit_freq / hz;
	if (full_div == 0) {
		full_div = 1; // PIT interprets 0 as 0xffff
	}
	if (full_div > hw_max_divider) {
		pit_ = 100;
		restart_ = full_div / 100;
	} else {
		pit_ = bitmask(full_div, 0, 0xffffu);
		restart_ = 0;
	}
	counter_ = restart_;
}
