#pragma once

#include <cstddef.hpp>

#include <bolgenos-ng/x86/scheduler.hpp>
#include <bolgenos-ng/x86/multitasking.hpp>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mmu.hpp>

namespace x86 {

template<size_t N, bool clear_stack = true>
class SimpleHwScheduler
{
	static_assert(N >= 2);
public:

	[[noreturn]] void replace_current_task() {
		RAISE_NOT_IMPLEMENTED;
	}

	const Task* build_task(task_routine routine) {
		auto task = allocate_task();
		if (task == nullptr) {
			panic("no free tasks");
		}
		return task;
	}

	void switch_to(const Task* task) {
		RAISE_NOT_IMPLEMENTED;
	}

	void destroy_task(const Task* task) {
		if (task == nullptr) {
			return;
		}
		task = {};
		if (clear_stack) {
			task->clear_stack();
		}
	}
private:
	Task* allocate_task() {
		for (x86::Task& task: _tasks) {
			if (task.available) {
				task.available = false;
				return &task;
			}
		}
		return nullptr;
	}

	static constexpr size_t _n_tasks = N;
	x86::Task _tasks[N];
};

}
