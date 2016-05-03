#pragma once


#include <bolgenos-ng/irq.hpp>


namespace m4 {


namespace fill_idt {


void fill_idt(irq::gate_t idt[]);


} // namespace fill_idt


} // namespace m4
