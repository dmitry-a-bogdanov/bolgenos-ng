#include <x86/scheduler.hpp>

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <threading/threading.hpp>
#include <bolgenos-ng/memory.hpp>
#include <x86/cpu.hpp>

using namespace lib;

x86::Scheduler::Scheduler() = default;

[[maybe_unused]] void x86::Scheduler::schedule_forever()
{
	irq::enable();
	cwarn << "===== STARTED SCHEDULING =====" << endl;
	while (true) {
		for (auto& task_ptr: _tasks) {
			if (task_ptr == _scheduler_task) {
				continue;
			}
			cinfo << "Scheduling to task [" << task_ptr->name() << "]" << endl;
			switch_to(task_ptr);
		}
	}
}

void scheduling_task_routine(void *arg) {
	static_cast<x86::Scheduler *>(arg)->schedule_forever();
}

void x86::Scheduler::init_multitasking(observer_ptr<GDT> gdt, x86::task_routine* main_continuation)
{
	[[gnu::thiscall]] void (*run_ptr)(Task *);
	run_ptr = &Task::wrapperForRun;

	static_assert(sizeof(run_ptr) == sizeof(_entry_point_for_tasks));
	lib::copy_n(reinterpret_cast<const char *>(&run_ptr), sizeof(run_ptr),
		    reinterpret_cast<char *>(&_entry_point_for_tasks));

	cinfo << "common entry point: " << hex << _entry_point_for_tasks << dec << endl;

	Task* main_cont = thr::with_lock<thr::RecursiveIrqLocker>([&]{
		_gdt = gdt;
		Task* scheduler_task = create_task(scheduling_task_routine, this, "scheduler");
		_scheduler_task = scheduler_task;

		auto m = create_task(main_continuation, nullptr, "main");

		_gdt->reload_table();

		return m;
	});

	_current = _scheduler_task;

	cinfo << "continue to new main" << endl;

	switch_to(_scheduler_task); // switch to itself to fill task data

	switch_to(main_cont);

	yield();

	schedule_forever();

	panic("couldn't start scheduling");
}

struct [[gnu::packed]] NewTaskStack {
	// bottom
	void* ebp2;
//	uint32_t edi;
//	uint32_t esi;
//	uint32_t ebx;
	x86::EFlags flags;
	void* ebp1;
	void (*eip)();
	x86::Task* task;
	// top
};

static_assert(sizeof(NewTaskStack) == 20);

x86::Task* x86::Scheduler::create_task(x86::task_routine* routine, void* arg, const char* name)
{
	auto* task = new Task{routine, arg, name};
	_tasks.push_front(task);

	uint16_t tssd_idx = _gdt->push_back(kernel_task_descriptor(task));
	uint16_t task_selector = segment_selector(tssd_idx, TableIndicator::GLOBAL, ProtectionRing::kernel);
	task->segment_selector(task_selector);
	//_gdt->reload_table();


	auto* new_task_stack = reinterpret_cast<NewTaskStack*>(task->_esp) - 1;
	new_task_stack->eip = Task::start_on_new_frame;
	cinfo << "eip: " << hex << reinterpret_cast<void*>(new_task_stack->eip) << dec << endl;
	new_task_stack->task = task;
	new_task_stack->ebp1 = reinterpret_cast<lib::byte *>(task->_stack) - 8;
	new_task_stack->ebp2 = new_task_stack->ebp1;
	new_task_stack->flags = Processor::flags();
	task->_esp = new_task_stack;
	return task;
}

void x86::Scheduler::switch_to(x86::Task* task)
{
	auto prev = _current;
	uint16_t selector = task->segment_selector();
	cinfo << "switching from " << (prev ? prev->name() : "null")
	      << " to task [" << task->name() << "]:" << selector << endl;
	cinfo << "task info: " << *task << endl;
	//x86::Processor::switch_task_to(selector);
	_current = task;
	switch_tasks_impl(prev, task);
	cinfo << "returned from switch" << endl;
}

[[gnu::cdecl, gnu::noinline]]
void x86::Scheduler::switch_tasks_impl(x86::Task* prev,
				       x86::Task* next) {
	asm(
"pushfl\n\t"
//"push %%ebx\n\t"
//"push %%esi\n\t"
//"push %%edi\n\t"
"push %%ebp\n\t"

"mov %%esp, %[prev_esp]\n\t"
"mov %[next_esp], %%esp\n\t"

"bolgenos_ng_task_switched:\n\t"
"pop %%ebp\n\t"
//"pop %%edi\n\t"
//"pop %%esi\n\t"
//"pop %%ebx\n\t"
"popfl\n\t"
: [prev_esp] "=m"(prev->_esp)
: [next_esp] "m"(next->_esp)
:
);

}

void x86::Scheduler::yield()
{
	cinfo << "yeilding" << endl;
	switch_to(_scheduler_task);
}

