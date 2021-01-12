#pragma once

#include <cstddef.hpp>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/page.hpp>

#include "tss.hpp"

namespace lib {
class ostream;
}

namespace x86 {

using task_routine = void (void *);

class Scheduler;

struct Task {
	TSS tss{};
	lib::byte* stack{};
	uint16_t segment_selector{0};
	char name[16]{"<unknown>"};
	void *arg{nullptr};
	task_routine* routine{nullptr};

	void run();

	[[gnu::thiscall]] static void wrapperForRun(Task* task);
};

lib::ostream& operator<<(lib::ostream& out, const Task& task);

} // namespace x86
