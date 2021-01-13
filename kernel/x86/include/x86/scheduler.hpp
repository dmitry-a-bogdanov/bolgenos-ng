#pragma once

#include <cstddef.hpp>

#include <ext/memory.hpp>
#include <forward_list.hpp>

#include "task.hpp"
#include "gdt.hpp"

namespace x86 {

class Scheduler
{
public:
	Scheduler();
	Scheduler(const Scheduler&) = delete;
	Scheduler(Scheduler&&) = delete;
	Scheduler& operator=(const Scheduler&) = delete;
	Scheduler& operator=(Scheduler&&) = delete;

	[[noreturn]]
	void init_multitasking(lib::observer_ptr<GDT> gdt, task_routine* main_continuation);

	Task* create_task(task_routine* routine, void* arg, const char* name = nullptr);
	void yield();

	[[maybe_unused]] [[noreturn]] [[gnu::thiscall]]
	void schedule_forever();
private:
	static constexpr size_t _n_tasks = 128;

	void switch_to(Task* task);

	lib::observer_ptr<GDT> _gdt{nullptr};
	lib::forward_list<x86::Task*> _tasks{};
	x86::Task* _scheduler_task{nullptr};
	lib::byte* _entry_point_for_tasks{nullptr};
};

}
