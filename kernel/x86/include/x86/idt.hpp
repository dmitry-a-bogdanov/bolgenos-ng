#pragma once

#include <array.hpp>
#include <cstdint.hpp>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/compiler.h>

#include "gate.hpp"

namespace x86 {

using GlobalIrqHandler = void (irq::irq_t vector, irq::stack_ptr_t);

class IDT {
public:
	IDT();
	void reload_table();
	static GlobalIrqHandler* set_global_handler(GlobalIrqHandler* handler);
private:
	alignas(cpu_alignment) lib::array<Gate, irq::NUMBER_OF_LINES> _idt;
	alignas(cpu_alignment) table_pointer _idt_pointer;
};

lib::ostream& operator<<(lib::ostream& out, const Gate& gate);

} // namespace x86
