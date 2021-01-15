#include <x86/idt.hpp>

#include <atomic.hpp>

using namespace lib;
using namespace x86;

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

template<class I, I ... Is>
auto make_gates(integer_sequence<I, Is...>)
{
	return array{Gate::interrupt_gate(&_asm_irq_handler<Is>)...};
}

template<size_t N>
array<Gate, N> make_gates()
{
	return make_gates(make_index_sequence<N>());
}

atomic<GlobalIrqHandler*> global_handler;

} // namespace


/// C IRQ dispatcher. The function to be called from ASM handlers.
extern "C"
[[maybe_unused, gnu::regparm(0), gnu::cdecl]]
void c_irq_dispatcher_(irq::irq_t vector, void* frame) {
	auto handler = global_handler.load();
	if (handler != nullptr) {
		handler(vector, frame);
	}
}

lib::ostream& x86::operator<<(lib::ostream& out, const Gate& gate) {
	switch (gate.type()) {
	case GateType::task:
		return out << "Task{}";
	case GateType::interrupt:
		return out << "Interrupt{}";
	case GateType::trap:
		return out << "Trap{}";
	default:
		unreachable();
	}
}

IDT::IDT()
	: _idt{make_gates<irq::NUMBER_OF_LINES>()},
	  _idt_pointer{static_cast<uint16_t>(_idt.size() * sizeof(Gate) - 1), _idt.data()}
{}

void IDT::reload_table()
{
	asm volatile("lidt %0"::"m" (_idt_pointer));
}

GlobalIrqHandler* IDT::set_global_handler(GlobalIrqHandler *handler) {
	return global_handler.exchange(handler);
}
