#pragma once

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/device.hpp>

namespace ps2 {


// forward declaration
class IPS2Line;


/// \brief Descriptor for PS/2 device.
///
/// ps_2dev structure describes ps2 device driver and contains pointer to
/// functions for handling device.
class IPS2Device {
public:
	virtual probe_ret_t probe(IPS2Line *) = 0; /*!< function that checks that
			* PS/2 device can be handled by the driver that
			* provides this ps2_dev structure.
			*/

	/// Function that will be called when receiving IRQ from the device
	/// which is managed by this driver.
	virtual irq::IRQHandler::status_t handle_irq() = 0;

	virtual ~IPS2Device() {}
};


} // namespace ps2
