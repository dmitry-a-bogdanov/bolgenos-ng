#pragma once

#include "irq.hpp"
#include "stdtypes.hpp"


namespace devices {


/// \brief Singleton base class for PIC devices.
///
/// Structure that describes generic PIC device. It can be used for
/// handling PIC device line PIC 8259, APIC or X2APIC.
class InterruptController {
public:
	InterruptController(const InterruptController &) = delete;
	InterruptController(InterruptController&&) = delete;
	InterruptController& operator =(const InterruptController &) = delete;
	InterruptController& operator =(InterruptController&&) = delete;

	static InterruptController *instance();



	/// Function that initializes PIC device.
	virtual void initialize_controller() = 0;

	/// Function that send "End of interrupt" message to generic PIC device
	/// for given IRQ line.
	virtual void end_of_interrupt(irq::irq_t irq) = 0;

	/// The first IRQ line number that can be received from the controller.
	virtual int min_irq_vector() const = 0;

protected:
	InterruptController() = default;
	virtual ~InterruptController() = default;

private:
	static InterruptController *_instance;
};



} // namespace pic

