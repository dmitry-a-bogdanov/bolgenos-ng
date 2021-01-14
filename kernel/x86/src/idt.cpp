#include <x86/idt.hpp>

using namespace lib;
using namespace x86;

static_assert(lib::is_standard_layout_v<gate_t>);

namespace {

template<int N>
[[gnu::aligned(16)]]
void _asm_irq_handler()
{
	asm(
	"pushal\n"
	"push %%esp\n"
	"pushl %0\n"
	"call c_irq_dispatcher_\n"
	"add $8, %%esp\n"
	"popal\n"
	"iret\n"
	:
	: "i"(N)
	);
}

using irq_handler_type = void();
using irq_handler_type_pointer = irq_handler_type*;

template<size_t N>
constexpr irq_handler_type_pointer make_asm_irq_handler()
{
	return &_asm_irq_handler<N>;
}

template<size_t N>
constexpr auto make_irq_gate()
{
	return gate_t::interrupt_gate(make_asm_irq_handler<N>());
}


template<size_t ... Is>
Array<gate_t, sizeof...(Is)> make_gates(index_sequence<Is...>)
{
	return {make_irq_gate<Is>()...};
}


template<size_t N>
Array<gate_t, N> declare_irq_gates()
{
	return make_gates(make_index_sequence<N>());
}

}


/// C IRQ dispatcher. The function to be called from ASM handlers.
extern "C"
[[maybe_unused, gnu::regparm(0), gnu::cdecl]]
void c_irq_dispatcher_(irq::irq_t vector, void* frame) {
	IDT::_dispatcher(vector, frame);
}

lib::ostream& x86::operator<<(lib::ostream& out, const gate_t& gate) {
	switch (gate.gate_kind_) {
	case gate_t::task:
		out << "Task{";
		break;
	case gate_t::interrupt:
		out << "Interrupt{";
		break;
	case gate_t::trap:
		out << "Trap{";
		break;
	default:
		out << "Unknown{";
		break;
	}
	return out << "}";
}

IDT::irq_dispatcher_func_t IDT::_dispatcher{nullptr};

IDT::IDT()
	: _idt{declare_irq_gates<irq::NUMBER_OF_LINES>()},
	  _idt_pointer{static_cast<uint16_t>(_idt.size() * sizeof(gate_t) - 1), _idt.data()}
{
}

void IDT::reload_table(IDT::irq_dispatcher_func_t irq_dispatcher)
{
	_dispatcher = irq_dispatcher;
	asm volatile("lidt %0"::"m" (_idt_pointer));
}

