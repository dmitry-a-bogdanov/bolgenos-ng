#pragma once

#include <cstddef.hpp>

#include "multitasking.hpp"

namespace x86 {

using task_routine = void ();

class Scheduler
{
public:
	static constexpr size_t TASKS = x86::TASKS;
};

}
