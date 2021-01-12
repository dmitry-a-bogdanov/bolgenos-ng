#include <x86/scheduler.hpp>

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <threading/threading.hpp>
#include <cstring.hpp>
#include <bolgenos-ng/memory.hpp>

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
			cinfo << "Scheduling to task [" << task_ptr->name << "]" << endl;
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
	auto task = allocate_task();
	if (task == nullptr) {
		panic("no free tasks");
	}
	task->routine = routine;
	task->arg = arg;

	auto& tss = task->tss;
	tss.instruction_ptr = _entry_point_for_tasks;
	tss._segment_registers = x86::tss::SegmentRegistersPack(
		x86::KERNEL_CODE_SEGMENT_SELECTOR,
		x86::KERNEL_DATA_SEGMENT_SELECTOR
	);
	tss._gp_registers_pack = {};
	tss._gp_registers_pack.ebp = task->stack;
	tss._gp_registers_pack.esp = task->stack;
	tss._gp_registers_pack.ecx = reinterpret_cast<uintptr_t>(task);
	tss.stack[0] = {x86::KERNEL_DATA_SEGMENT_SELECTOR, task->stack};
	tss.stack[1] = tss.stack[2] = {};

	uint16_t tssd_idx = _gdt->push_back(kernel_task_descriptor(task));
	uint16_t task_selector = segment_selector(tssd_idx, TableIndicator::GLOBAL, ProtectionRing::kernel);
	task->segment_selector = task_selector;
	if (name != nullptr) {
		snprintf(task->name, sizeof(Task::name), name);
	}

	_gdt->reload_table();

	return task;
}

bool x86::Scheduler::is_initialized() const noexcept
{
	return (bool) _gdt;
}

x86::Task* x86::Scheduler::allocate_task()
{
	auto* task = new Task{};
	task->stack = reinterpret_cast<lib::byte*>(memory::alloc_pages(16));
	_tasks.push_front(task);
	return task;
}

void x86::Scheduler::switch_to(x86::Task* task)
{
	uint16_t selector = task->segment_selector;
	cinfo << "switching to task [" << task->name << "]:" << selector << endl;
	cinfo << "task info: " << *task << endl;
	//x86::switch_to(task->segment_selector);
	switch_to__(selector);
	cinfo << "returned from switch" << endl;
}

void x86::Scheduler::switch_to__(uint16_t selector) {
	struct {unsigned int offset; unsigned short segment;} dest{};
	dest.offset = 0x0;
	dest.segment = selector;
	asm volatile (
	"ljmp *%0\n"
	"1:" :: "m"(dest));
}

void x86::Scheduler::yield()
{
	cinfo << "yeilding" << endl;
	switch_to(_scheduler_task);
}

