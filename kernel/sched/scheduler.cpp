#include "scheduler.hpp"

#include <bolgenos-ng/irq.hpp>
#include <threading/with_lock.hpp>
#include <bolgenos-ng/memory.hpp>
#include <x86/cpu.hpp>

using namespace lib;
using namespace sched;
using namespace x86;


void scheduling_task_routine(void *arg) {
	static_cast<sched::Scheduler *>(arg)->schedule_forever();
}

sched::Scheduler::Scheduler(task_routine* main_continuation)
{
	thr::with_irq_lock([&]{
		if (main_continuation == nullptr) {
			panic("no main specified");
		}
		auto scheduler_task = create_task(scheduling_task_routine, this, "scheduler");
		_scheduler_task = scheduler_task;
		create_task(main_continuation, nullptr, "main");
		_current = _scheduler_task;
	});
};

[[maybe_unused]] void sched::Scheduler::schedule_forever()
{
	if (!irq::is_enabled()) {
		panic("started scheduling with blocked interrupts");
	}
	WARN << "===== STARTED SCHEDULING =====" << endl;
	constexpr bool debug_irq = false;
	while (true) {
		irq::disable(debug_irq);
		for (auto task_ptr: _tasks) {
			if (!should_schedule(task_ptr)) {
				continue;
			}
			INFO << "Scheduling to task [" << task_ptr->name() << "]" << endl;
			irq::enable(debug_irq);
			// switch_to knows better how to deal with interrupts
			switch_to(task_ptr);
			irq::disable(debug_irq);
		}
		handle_finished_tasks();
		irq::enable(debug_irq);
	}
}

void sched::Scheduler::start_scheduling()
{
	INFO << "switching into itself to fill task data" << endl;
	switch_to(_scheduler_task);

	schedule_forever();

	panic("Scheduling finished");
}

struct [[gnu::packed]] NewTaskStack {
	void* ebp2;
	x86::EFlags flags;
	void* ebp1;
	void (*eip)();
	Task* task;
};

static_assert(sizeof(NewTaskStack) == 20);

Task* sched::Scheduler::create_task(task_routine* routine, void* arg, const char* name)
{
	auto* task = new Task{this, routine, arg, name};
	_tasks.insert(task);

	auto* new_task_stack = reinterpret_cast<NewTaskStack*>(task->_esp) - 1;
	new_task_stack->eip = Task::start_on_new_frame;
	INFO << "eip: " << hex << reinterpret_cast<void*>(new_task_stack->eip) << dec << endl;
	new_task_stack->task = task;
	new_task_stack->ebp1 = reinterpret_cast<lib::byte *>(task->_stack) - 8;
	new_task_stack->ebp2 = new_task_stack->ebp1;
	new_task_stack->flags = Processor::flags();
	task->_esp = new_task_stack;
	return task;
}

void sched::Scheduler::switch_to(Task* task)
{
	if (!irq::is_enabled()) {
		panic("switching while interrupts are disabled");
	}
	irq::disable(false);

	auto prev = _current;
	INFO << "Switch: [" << prev->name() << "](" << prev->id() << ")"
	      << " -> " << *task << endl;
	_current = task;
	switch_tasks_impl(prev, task);
	irq::enable(false);
	INFO << "returned from switch" << endl;
}

[[gnu::cdecl, gnu::noinline]]
void sched::Scheduler::switch_tasks_impl(Task* prev, Task* next) {
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

void sched::Scheduler::yield()
{
	INFO << "yielding" << endl;
	switch_to(_scheduler_task);
}

bool sched::Scheduler::should_schedule(const Task* task)
{
	return task != _scheduler_task &&
		!task->finished();
}

void sched::Scheduler::handle_exit(Task* task)
{
	thr::with_irq_lock([&]() {
		_finished_tasks.push_front(task);
	});
	yield();
}

void Scheduler::handle_finished_tasks()
{
	thr::with_irq_lock([&]() {
		for (auto task_ptr: _finished_tasks) {
			_tasks.remove(task_ptr);
			delete task_ptr;
		}
		_finished_tasks.clear();
	});
}
