#pragma once

#include <bolgenos-ng/irq.hpp>


namespace m4 {


/// Type of pointer to IRQ dispatcher function.
using irq_dispatcher_func_t = void (*)(irq::irq_t vector,
		irq::stack_ptr_t frame);


/// Get pointer to preset interrupt descriptor table.
void* get_idt(irq_dispatcher_func_t function);


} // namespace m4
