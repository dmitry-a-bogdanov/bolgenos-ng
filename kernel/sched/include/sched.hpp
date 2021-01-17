#pragma once

#include <sched/task.hpp>

namespace sched {

void yield();

Task* create_task(task_routine* routine, void* arg, const char* name = nullptr);

namespace details {

void init_scheduling(task_routine* main_continuation);

} // namespace details

} // namespace sched

