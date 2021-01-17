#pragma once

#include <atomic.hpp>
#include <cstddef.hpp>

#include <ext/memory.hpp>
#include <forward_list.hpp>

#include "task.hpp"
#include "gdt.hpp"

namespace x86 {

void yield();

class Scheduler
{
public:
	Scheduler();
	Scheduler(const Scheduler&) = delete;
	Scheduler(Scheduler&&) = delete;
	Scheduler& operator=(const Scheduler&) = delete;
	Scheduler& operator=(Scheduler&&) = delete;

	[[noreturn]]
	void init_multitasking(task_routine* main_continuation);

	Task* create_task(task_routine* routine, void* arg, const char* name = nullptr);
	void yield();

	[[maybe_unused]] [[noreturn]] [[gnu::thiscall]]
	void schedule_forever();

	[[gnu::cdecl]]
	static void switch_tasks_impl(Task* from, Task* to);
private:
	void switch_to(Task* task);

	lib::forward_list<x86::Task*> _tasks{};
	x86::Task* _scheduler_task{nullptr};
	x86::Task* _current{nullptr};
};

}
