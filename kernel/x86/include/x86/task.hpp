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

struct Task {
	TSS tss{};
	bool available = true;
	lib::byte _stack_storage[PAGE_SIZE*16]{};
	lib::byte stack[0];
	uint16_t segment_selector{0};
	char name[16]{"<unknown>"};
	void *arg{nullptr};
	task_routine* routine{nullptr};

	void run();

	void clear_stack() {
		for (auto& byte: _stack_storage) {
			byte = static_cast<lib::byte>(0xaa);
		}
	}

	[[gnu::thiscall]] static void wrapperForRun(Task* task) {
		task->run();
	}
};

lib::ostream& operator<<(lib::ostream& out, const Task& task);

} // namespace x86
