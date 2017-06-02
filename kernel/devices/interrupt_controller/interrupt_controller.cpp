#include <bolgenos-ng/interrupt_controller.hpp>

#include "pic_8259.hpp"

devices::InterruptController *devices::InterruptController::instance()
{
	if (!_instance) {
		_instance = new devices::PIC8259();
	}
	return _instance;
}


devices::InterruptController *devices::InterruptController::_instance = nullptr;
