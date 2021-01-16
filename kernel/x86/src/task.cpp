#include <x86/task.hpp>

#include <ext/scoped_format_guard.hpp>
#include <atomic.hpp>
#include <cstring.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/irq.hpp>

using namespace x86;
using namespace lib;

static lib::atomic<uint32_t> next_task_id{0};



lib::ostream& x86::operator<<(lib::ostream& out, TaskId id) {
	ScopedFormatGuard guard{out};
	return out << dec << static_cast<underlying_type_t<TaskId>>(id);
}

static TaskId allocate_task_id() {
	return static_cast<TaskId>(next_task_id++);
}

static_assert(lib::is_standard_layout_v<x86::Task>);

lib::ostream& x86::operator<<(lib::ostream& out, const Task& task) {
	ScopedFormatGuard guard{out};
	return out << "Task[" << task.name() << "](" << task.id() << ")"
		<< "{.esp=" << hex << task.esp()
		<< ",.stack=" << task.stack()
		<< "}";
}

Task::Task(task_routine* routine, void* arg, const char* name_) :
	_routine{routine},
	_arg{arg},
	_id{allocate_task_id()}
{
	static constexpr size_t stack_pages = 16;
	_stack = reinterpret_cast<byte*>(memory::alloc_pages(stack_pages));
	_esp = _stack + PAGE_SIZE*stack_pages;
	name(name_);
}

void x86::Task::run()
{
	irq::enable(false);
	this->_routine(this->_arg);
}

[[gnu::thiscall]] void x86::Task::wrapperForRun(Task* task) {
	cinfo << "starting" << *task << endl;
	task->run();
	cnotice << "Finished task " << *task << endl;
}

void Task::name(const char* name)
{
	strncpy(_name, name, sizeof(_name) - 1);
	_name[sizeof(_name) - 1] = '\0';
}

Task::~Task()
{
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