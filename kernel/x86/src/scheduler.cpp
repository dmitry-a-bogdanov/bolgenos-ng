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

	thr::with_lock<thr::RecursiveIrqLocker>([&]{
		_gdt = gdt;
		Task* scheduler_task = create_task(scheduling_task_routine, this, "scheduler");
		_scheduler_task = scheduler_task;

		create_task(main_continuation, nullptr, "main");

		_gdt->reload_table();
	});

	yield();

	panic("couldn't start scheduling");
}

x86::Task* x86::Scheduler::create_task(x86::task_routine* routine, void* arg, const char* name)
{
	auto* task = new Task{routine, arg, name};
	_tasks.push_front(task);

	uint16_t tssd_idx = _gdt->push_back(kernel_task_descriptor(task));
	uint16_t task_selector = segment_selector(tssd_idx, TableIndicator::GLOBAL, ProtectionRing::kernel);
	task->segment_selector(task_selector);
	_gdt->reload_table();

	return task;
}

void x86::Scheduler::switch_to(x86::Task* task)
{
	uint16_t selector = task->segment_selector();
	cinfo << "switching to task [" << task->name() << "]:" << selector << endl;
	cinfo << "task info: " << *task << endl;
	x86::Processor::switch_task_to(selector);
	cinfo << "returned from switch" << endl;
}


void x86::Scheduler::yield()
{
	cinfo << "yeilding" << endl;
	switch_to(_scheduler_task);
}

