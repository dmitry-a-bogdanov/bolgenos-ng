#include "include/sched.hpp"

#include <memory.hpp>

#include "scheduler.hpp"

using namespace lib;

static unique_ptr<sched::Scheduler> instance;

void sched::yield() {
	instance->yield();
}

sched::Task* sched::create_task(sched::task_routine* routine, void* arg, const char* name)
{
	return instance->create_task(routine, arg, name);
}


void sched::details::init_scheduling(task_routine* main_continuation) {
	instance = make_unique<Scheduler>(main_continuation);
	instance->start_scheduling();
}