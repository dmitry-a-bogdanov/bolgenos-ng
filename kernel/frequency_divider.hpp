#pragma once

#include <cstdint>


namespace pit {


/// Internal classes that may be used only inside \ref pit namespace.
namespace details {


/// \brief Timer frequency divider.
///
/// Frequency dividing consists of two stages: hardware and software.
///
/// Hardware dividing works in the following way: OS writes 2-bytes number to
/// the PIT chip. The PIT chip decrements this number until it became equal
/// to zero. Then it raises interrupt.
///
/// Software frequency dividing works in similar manner. The two values are
/// used: timeout of hardware timer and number of such hardware timer timeouts
/// till SW tick happens.
///
/// This behavior is implemented by the following variables: \ref pit_,
/// \ref counter_ and \ref restart_. \ref pit_ is an value for hardware timer.
/// \ref counter_ is a number of HW ticks till the next SW timer tick.
/// \ref restart_ is a number of HW ticks in one SW timer tick.
class FrequencyDivider {
public:

	/// Default constructor.
	FrequencyDivider() = default;


	/// Copy-constructing is denied.
	FrequencyDivider(const FrequencyDivider&) = delete;


	/// Copy-assignment is denied.
	FrequencyDivider& operator=(const FrequencyDivider&) = delete;


	/// Destructor.
	~FrequencyDivider();


	/// \brief Set frequency of divider.
	///
	/// The functions sets the frequency of the frequency divider. It will
	/// be the frequency how often \ref do_tick returns true.
	///
	/// \param hz Desired frequency.
	/// \param hw_pit_freq Frequency of hardware timer.
	/// \param hw_max_divider Maximal hardware frequency divider.
	void set_frequency(unsigned long hz, const unsigned long hw_pit_freq,
			const unsigned long hw_max_divider);


	/// Get pit timeout for correct working of frequency divider.
	uint16_t pit_timeout() const;


	/// \brief Do hardware tick.
	///
	/// The function notifies frequency divider that hardware ticks happens
	/// and returns if boolean flags that shows whether software timer tick
	/// is needed or not.
	///
	/// \return true if software tick is needed; false otherwise.
	inline bool do_tick() {
		if (counter_ != 0) {
			--counter_;
			return false;
		} else {
			counter_ = restart_;
			return true;
		}
	}


	/// \brief Is low frequency.
	///
	/// Flag shows hardware timer is not enough since frequency is too low
	/// for dividing only by hardware timer and software dividing will be
	/// used in addition. It will decrease accuracy of timer.
	///
	/// \return true if software dividing will be used; false otherwise.
	bool is_low_frequency() const;
private:


	/// Restart counter for hardware timer.
	uint16_t pit_ = 0x0;


	/// Current number of hardware ticks until software tick.
	unsigned long counter_ = 0;


	/// Restart counter of software timer.
	unsigned long restart_ = 0;
};


} // namespace detail


} // namespace pit
