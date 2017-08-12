#pragma once

#include <bolgenos-ng/irq.hpp>


namespace bolgenos::irq
{

extern bolgenos::irq::gate_t idt[::irq::NUMBER_OF_LINES];

}

