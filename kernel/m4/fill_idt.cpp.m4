m4_changequote([,])
m4_include(utils.m4)

m4_divert(-1)

m4_define([____asm], [____irqh_asm_$1])
m4_define([____c], [____irqh_c_$1])
m4_define([____disp], [irq_dispatcher])

m4_define([____asm_definition], [extern "C" void ____asm($1)();
  asm(
    ".align 16\n"
    "____asm($1):\n"
      "pushal\n"
      "push %esp\n"
      "pushl $$1\n"
      "call ____disp\n"
      "add $[]4, %esp\n"
      "pop %esp\n"
      "popal\n"
      "iret\n"
  );

])


m4_define([____irqh_definitions], [____for([____num], [0], [$1], [____asm_definition(____num)])])



m4_define([____fill_entry], [[$1][[$2]] = irq::gate_t::interrupt_gate(____asm($2));
])

m4_define([____fill_idt], [____for([____num], 0, $2, [____fill_entry($1, ____num)])])

m4_divert[]m4_dnl
#include <m4/fill_idt.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/pic_common.hpp>

m4_define([__max_irq], 48)

____irqh_definitions(__max_irq)

void m4::fill_idt::fill_idt(irq::gate_t idt[[]]) {
____fill_idt(idt, __max_irq)
}

