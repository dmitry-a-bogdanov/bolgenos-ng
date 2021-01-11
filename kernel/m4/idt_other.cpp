#include <m4/idt.hpp>


lib::ostream& operator<<(lib::ostream& out, const gate_t& gate) {
	out << "gate: " << uint32_t{gate.gate_kind_} << " ";
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
	}
	return out << "}";
}
