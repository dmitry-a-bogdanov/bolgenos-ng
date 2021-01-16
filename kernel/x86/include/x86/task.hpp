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
public:
	constexpr Task() = default;

	Task(task_routine* routine, void* arg, const char *name = nullptr);
	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

	~Task();

	void run();

	void name(const char* name);
	const char* name() const { return _name; }

	TSS& tss() { return _tss; }
	const TSS& tss() const { return _tss; }

	void segment_selector(uint16_t selector) {
		_segment_selector = selector;
	}

	uint16_t segment_selector() const {
		return _segment_selector;
	}

	[[gnu::thiscall]] static void wrapperForRun(Task* task);

	[[gnu::naked]] static void start_on_new_frame();

	void* esp() const { return _esp; }
	void* stack() const { return _stack; }

private:
	TSS _tss{};

	void* _esp{nullptr};

	lib::byte* _stack{};
	uint16_t _segment_selector{0};
	char _name[16]{"<unknown>"};
	void* _arg{nullptr};
	task_routine* _routine{nullptr};

	friend class Scheduler;
};

lib::ostream& operator<<(lib::ostream& out, const Task& task);

} // namespace x86
