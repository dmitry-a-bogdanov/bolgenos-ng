#pragma once

#include "irq.hpp"
#include "stdtypes.hpp"


namespace pic {


/// \brief Generic PIC device.
///
/// Structure that describes generic PIC device. It can be used for
/// handling PIC device line PIC 8259, APIC or X2APIC.
struct pic_device {
public:
	pic_device() = default;


	pic_device(const pic_device &) = delete;


	pic_device& operator =(const pic_device &) = delete;


	/// Function that initializes PIC device.
	virtual void setup() = 0;


	/// Function that send "End of interrupt" message to generic PIC device
	/// for given IRQ line.
	virtual void end_of_interrupt(irq::irq_t irq) = 0;


	virtual ~pic_device() {}


	/// The first IRQ line number that can be received from the PIC chip.
	int min_irq_vector = 0;
};


// \brief Pointer to PIC device structure that is used on the system.
extern pic_device *system_pic;


/// \brief The first IRQ line number that can be received from the PIC chip.
///
/// Alias to system_pic->min_irq_vector.
inline int min_pic_irq() {
	return pic::system_pic->min_irq_vector;
}


/// \brief Send "End of interrupt" message to system PIC device.
///
/// Alias to system_pic->end_of_interrupt
inline void end_of_irq(irq::irq_t vec) {
	pic::system_pic->end_of_interrupt(vec);
}


} // namespace pic

