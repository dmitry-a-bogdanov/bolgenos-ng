#include <x86/task.hpp>

#include <algorithm.hpp>
#include <cstring.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/mmu.hpp>

using namespace x86;
using namespace lib;

static_assert(lib::is_standard_layout_v<x86::Task>);

lib::ostream& x86::operator<<(lib::ostream& out, const Task& task) {
	return out << "{.tss = " << task.tss() << "}";
}

static byte* get_entry_point() {
	[[gnu::thiscall]] void (*run_ptr)(Task *);
	run_ptr = &Task::wrapperForRun;

	byte* entry_point;
	static_assert(sizeof(run_ptr) == sizeof(entry_point));
	copy_n(reinterpret_cast<const char *>(&run_ptr), sizeof(run_ptr),
		    reinterpret_cast<char *>(&entry_point));
	return entry_point;
}

Task::Task(task_routine* routine, void* arg, const char* name_) :
	_tss{},
	_arg{arg},
	_routine{routine}
{
	static_assert(offsetof(x86::Task, _tss) == 0);
	_stack = reinterpret_cast<byte*>(memory::alloc_pages(16));
	auto& gp_registers = _tss._gp_registers_pack;

	_tss._segment_registers = x86::tss::SegmentRegistersPack(
		x86::KERNEL_CODE_SELECTOR,
		x86::KERNEL_DATA_SELECTOR
	);

	gp_registers.ecx = reinterpret_cast<lib::uintptr_t>(this);
	gp_registers.esp = gp_registers.ebp = _stack;
	_tss.stack[0] = {x86::KERNEL_DATA_SELECTOR, _stack};
	_tss.stack[1] = _tss.stack[2] = {};
	_tss.instruction_ptr = get_entry_point();
	name(name_);
}

void x86::Task::run()
{
	this->_routine(this->_arg);
}

[[gnu::thiscall]] void x86::Task::wrapperForRun(Task* task) {
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
