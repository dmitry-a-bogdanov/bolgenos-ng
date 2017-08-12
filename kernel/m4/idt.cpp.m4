#include <m4/idt.hpp>


m4_changequote([,])
m4_include(utils.m4)

m4_divert(-1)

m4_define([____asm], [irqh_asm_$1])
m4_define([____c], [irqh_c_$1])

m4_define([____asm_definition], [extern "C" void ____asm($1)();
asm(
	".align 16\n"
	"____asm($1):\n"
		"pushal\n"
		"push %esp\n"
		"pushl $$1\n"
		"call dispatcher_caller\n"
		"add $[]8, %esp\n"
		"popal\n"
		"iret\n"
);

])


m4_define([____irqh_definitions], [____for([____num], [0], [$1], [____asm_definition(____num)])])

m4_define([idt_define_entry],	bolgenos::irq::gate_t::interrupt_gate(____asm($2))[,]
)
m4_define([idt_define_idt], [____for([____num], 0, $2, [idt_define_entry($1, ____num)])])
m4_divert[]m4_dnl
m4_define([__max_irq], 256)

namespace
{


/// C IRQ dispatcher. The function to be called from ASM handlers.
extern "C"
void __attribute__((regparm(0), cdecl)) dispatcher_caller(irq::irq_t vector, void* frame)
{
	bolgenos::irq::get_dispatcher()(vector, frame);
}

____irqh_definitions(__max_irq)

}
// namespace

bolgenos::irq::gate_t bolgenos::irq::idt[[::irq::NUMBER_OF_LINES]] alignas(cpu_alignment) =
{
idt_define_idt(dummy, __max_irq)
};

