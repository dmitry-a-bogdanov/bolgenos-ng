#include "include/sched/task.hpp"

#include <config.h>
#include <ext/scoped_format_guard.hpp>
#include <atomic.hpp>
#include <cstring.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/irq.hpp>

#include "scheduler.hpp"

using namespace lib;
using namespace sched;
using namespace x86;

static lib::atomic<uint32_t> next_task_id{0};



lib::ostream& sched::operator<<(lib::ostream& out, TaskId id) {
	ScopedFormatGuard guard{out};
	return out << dec << static_cast<underlying_type_t<TaskId>>(id);
}

static TaskId allocate_task_id() {
	return static_cast<TaskId>(next_task_id++);
}

lib::ostream& sched::operator<<(lib::ostream& out, const Task& task) {
	ScopedFormatGuard guard{out};
	return out << "Task[" << task.name() << "](" << task.id() << ")"
		<< "{.esp=" << hex << task.esp()
		<< ",.stack=" << task.stack()
		<< "}";
}

Task::Task(Scheduler* creator, task_routine* routine, void* arg, const char* name_) :
	_routine{routine},
	_arg{arg},
	_id{allocate_task_id()},
	_scheduler{creator}
{
	static constexpr size_t stack_pages = 16;
	_stack = reinterpret_cast<byte*>(memory::alloc_pages(stack_pages));
	_esp = _stack + PAGE_SIZE*stack_pages;
	name(name_);
}

void sched::Task::run()
{
	NOTICE << "Starting " << *this << endl;
	irq::enable(false);
	_routine(_arg);
	_exited.store(true);
	NOTICE << "Finished task " << *this << endl;
	_scheduler->handle_exit(this);
	while (true) {
		_scheduler->yield();
	}
}

[[gnu::thiscall]] void sched::Task::wrapperForRun(Task* task) {
	task->run();
}

void Task::name(const char* name)
{
	strncpy(_name, name, sizeof(_name) - 1);
	_name[sizeof(_name) - 1] = '\0';
}

Task::~Task()
{
	NOTICE << "Removing task " << *this << endl;
	memory::free_pages(_stack);
	_stack = nullptr;
}

void Task::start_on_new_frame()
{
	asm volatile(
		"pop %%ecx\n\t"
  		"call %P0\n\t"
  		:: "i"(wrapperForRun));
}