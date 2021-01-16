#include <x86/scheduler.hpp>

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <threading/threading.hpp>
#include <threading/with_lock.hpp>
#include <bolgenos-ng/memory.hpp>
#include <x86/cpu.hpp>

using namespace lib;

x86::Scheduler::Scheduler() = default;

[[maybe_unused]] void x86::Scheduler::schedule_forever()
{
	if (!irq::is_enabled()) {
		panic("started scheduling with blocked interrupts");
	}
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

void x86::Scheduler::init_multitasking(x86::task_routine* main_continuation)
{
	Task* main_cont = thr::with_lock<thr::RecursiveIrqLocker>([&]{
		Task* scheduler_task = create_task(scheduling_task_routine, this, "scheduler");
		_scheduler_task = scheduler_task;

		return create_task(main_continuation, nullptr, "main");
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
	void* ebp2;
	x86::EFlags flags;
	void* ebp1;
	void (*eip)();
	x86::Task* task;
};

static_assert(sizeof(NewTaskStack) == 20);

x86::Task* x86::Scheduler::create_task(x86::task_routine* routine, void* arg, const char* name)
{
	auto* task = new Task{routine, arg, name};
	_tasks.push_front(task);

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
	if (!irq::is_enabled()) {
		panic("switching while interrupts are disabled");
	}
	irq::disable(false);

	auto prev = _current;
	cinfo << "Switch: [" << prev->name() << "](" << prev->id() << ")"
	      << " -> " << *task << endl;
	_current = task;
	switch_tasks_impl(prev, task);
	irq::enable(false);
	cinfo << "returned from switch" << endl;
}

[[gnu::cdecl, gnu::noinline]]
void x86::Scheduler::switch_tasks_impl(x86::Task* prev,
				       x86::Task* next) {
	asm(
"pushfl\n\t"
"push %%ebp\n\t"

"mov %%esp, %[prev_esp]\n\t"
"mov %[next_esp], %%esp\n\t"

"bolgenos_ng_task_switched:\n\t"
"pop %%ebp\n\t"
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

