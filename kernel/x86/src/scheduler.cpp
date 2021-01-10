#include <x86/scheduler.hpp>

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mmu.hpp>

#include <cstring.hpp>

using namespace lib;
using namespace mmu;

x86::Scheduler::Scheduler() = default;

void x86::Scheduler::schedule_forever()
{
	irq::enable();
	cwarn << "STARTED SCHEDULING" << endl;
	while (true) {
		for (auto& task: _tasks) {
			if (&task == _scheduler_task) {
				continue;
			}
			if (!task.available) {
				cinfo << "Scheduling to task [" << task.name << "]" << endl;
				switch_to(&task);
			}
		}
	}
}

/**
 * expectes ecx is filled by scheduler object
 */
void scheduling_task_routine() {
	asm volatile("call _ZN3x869Scheduler16schedule_foreverEv\n\t");
}

void x86::Scheduler::init_multitasking(observer_ptr<GDT> gdt, x86::task_routine* main_continuation)
{
	_gdt = gdt;
	Task* scheduler_task = create_task(scheduling_task_routine, "scheduler");
	scheduler_task->tss._gp_registers_pack.ecx = reinterpret_cast<uintptr_t>(this);
	_scheduler_task = scheduler_task;

	create_task(main_continuation, "main");

	_gdt->reload_table();
	yield();

	panic("couldn't start scheduling");
}

x86::Task* x86::Scheduler::create_task(x86::task_routine* routine, const char* name)
{
	auto task = allocate_task();
	if (task == nullptr) {
		panic("no free tasks");
	}
	auto& tss = task->tss;
	tss.instruction_ptr = reinterpret_cast<byte*>(routine);
	tss._segment_registers = x86::tss::SegmentRegistersPack(
		mmu::KERNEL_CODE_SEGMENT_POINTER,
		mmu::KERNEL_DATA_SEGMENT_POINTER
	);
	tss._gp_registers_pack = {};
	tss._gp_registers_pack.ebp = task->stack;
	tss._gp_registers_pack.esp = task->stack;
	tss.stack[0] = {mmu::KERNEL_DATA_SEGMENT_POINTER, task->stack};
	tss.stack[1] = tss.stack[2] = {};

	uint16_t tssd_idx = _gdt->push_back(kernel_task_descriptor(task));
	uint16_t task_selector = segment_selector(tssd_idx, TableIndicator::GLOBAL, ring_kernel);
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
	for (x86::Task& task: _tasks) {
		if (task.available) {
			task.available = false;
			return &task;
		}
	}
	return nullptr;
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

