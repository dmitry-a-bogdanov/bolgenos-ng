#include <m4/idt.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/mem_utils.hpp>

namespace {


m4_changequote([,])
m4_include(utils.m4)

m4_divert(-1)

m4_define([____asm], [____irqh_asm_$1])
m4_define([____c], [____irqh_c_$1])

m4_define([____asm_definition], [extern "C" void ____asm($1)();
  asm(
    ".align 16\n"
    "____asm($1):\n"
      "pushal\n"
      "push %esp\n"
      "pushl $$1\n"
      "call ____disp\n"
      "add $[]8, %esp\n"
      "popal\n"
      "iret\n"
  );

])


m4_define([____irqh_definitions], [____for([____num], [0], [$1], [____asm_definition(____num)])])



m4_define([____fill_entry], [[$1][[$2]] = gate_t::interrupt_gate(____asm($2));
])

m4_define([____fill_idt], [____for([____num], 0, $2, [____fill_entry($1, ____num)])])

m4_divert[]m4_dnl

m4_define([__max_irq], 255)


/// Pointer to C++ IRQ dispatcher
m4::irq_dispatcher_func_t registered_dispatcher = nullptr;


/// C IRQ dispatcher. The function to be called from ASM handlers.
extern "C" void __attribute__((regparm(0), cdecl)) ____disp(irq::irq_t vector, void* frame) {
	registered_dispatcher(vector, frame);
}


____irqh_definitions(__max_irq)


static_assert(sizeof(gate_t) == irq::GATE_SIZE, "gate_t has wrong size");


gate_t idt[[irq::NUMBER_OF_LINES]] _irq_aligned_;


} // namespace


void* m4::get_idt(irq_dispatcher_func_t function) {
	registered_dispatcher = function;
____fill_idt(idt, __max_irq)
	return &idt;
}

