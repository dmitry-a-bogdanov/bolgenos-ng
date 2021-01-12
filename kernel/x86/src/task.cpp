#include <x86/task.hpp>
#include <bolgenos-ng/irq.hpp>

using namespace x86;

lib::ostream& x86::operator<<(lib::ostream& out, const Task& task) {
	return out << "{.tss = " << task.tss << "}";
}

void x86::Task::run()
{
	this->routine(this->arg);
}
