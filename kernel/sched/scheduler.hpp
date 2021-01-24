#pragma once

#include <atomic.hpp>
#include <cstddef.hpp>

#include <ext/memory.hpp>
#include <ext/intrusive_circular_list.hpp>
#include <forward_list.hpp>

#include <loggable.hpp>
#include <sched/task.hpp>

namespace sched {

class Scheduler: private Loggable("scheduler")
{
public:
	Scheduler(task_routine* main_continuation);

	Scheduler(const Scheduler&) = delete;

	Scheduler(Scheduler&&) = delete;

	Scheduler& operator=(const Scheduler&) = delete;

	Scheduler& operator=(Scheduler&&) = delete;

	[[noreturn]]
	void start_scheduling();

	Task* create_task(task_routine* routine, void* arg, const char* name = nullptr);

	void handle_exit(Task* task);

	void yield();

	[[maybe_unused]] [[noreturn]] [[gnu::thiscall]]
	void schedule_forever();

	[[gnu::cdecl]]
	static void switch_tasks_impl(Task* from, Task* to);

private:
	void switch_to(Task* task);

	bool should_schedule(const Task* task);
	void handle_finished_tasks();

	lib::CircularIntrusiveList<Task> _tasks{&Task::tasks_list_node};
	lib::forward_list<Task*> _finished_tasks{};
	Task* _scheduler_task{nullptr};
	Task* _current{nullptr};
};

} // namespace sched

