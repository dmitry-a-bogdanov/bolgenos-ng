#include <x86/task.hpp>

#include <bolgenos-ng/log.hpp>

using namespace x86;
using namespace lib;

static_assert(offsetof(x86::Task, tss) == 0);
static_assert(lib::is_standard_layout_v<x86::Task>);

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