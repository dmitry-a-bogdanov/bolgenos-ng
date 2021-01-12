#include <x86/task.hpp>
#include <bolgenos-ng/irq.hpp>

#include <bolgenos-ng/log.hpp>
#include <x86/scheduler.hpp>

using namespace x86;
using namespace lib;

lib::ostream& x86::operator<<(lib::ostream& out, const Task& task) {
	return out << "{.tss = " << task.tss << "}";
}

void x86::Task::run()
{
	this->routine(this->arg);
}

[[gnu::thiscall]] void x86::Task::wrapperForRun(Task* task) {
	task->run();
	cnotice << "Finished task " << *task << endl;
}