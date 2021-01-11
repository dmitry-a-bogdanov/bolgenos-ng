#pragma once

#include <cstddef.hpp>

#include <ext/memory.hpp>

#include "task.hpp"
#include "gdt.hpp"

namespace x86 {

using task_routine = void ();

class Scheduler
{
public:
	Scheduler();

	[[noreturn]]
	void init_multitasking(lib::observer_ptr<GDT> gdt, task_routine* main_continuation);

	Task* create_task(task_routine* routine, const char* name = nullptr);
	void yield();

	[[maybe_unused]] [[noreturn]] [[gnu::thiscall]]
	void schedule_forever();
private:
	static constexpr size_t _n_tasks = 128;

	[[nodiscard]]
	bool is_initialized() const noexcept;

	Task* allocate_task();
	void switch_to(Task* task);

	[[gnu::noinline]]
	void switch_to__(uint16_t selector);


	lib::observer_ptr<GDT> _gdt{nullptr};
	x86::Task _tasks[_n_tasks];
	x86::Task* _scheduler_task{nullptr};
};

}
