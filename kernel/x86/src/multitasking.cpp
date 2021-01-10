#include <x86/multitasking.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/irq.hpp>

using namespace mmu;

lib::ostream& x86::operator<<(lib::ostream& out, const Task& task) {
	return out << "{.tss = " << task.tss << "}";
}

