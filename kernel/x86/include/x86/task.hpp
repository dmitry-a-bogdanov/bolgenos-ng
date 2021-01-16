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

enum class TaskId: uint32_t {};

lib::ostream& operator<<(lib::ostream& out, TaskId id);

struct Task {
public:
	Task() = delete;

	Task(task_routine* routine, void* arg, const char *name = nullptr);
	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

	~Task();

	void run();

	void name(const char* name);
	const char* name() const { return _name; }

	[[gnu::thiscall]] static void wrapperForRun(Task* task);

	[[gnu::naked]] static void start_on_new_frame();

	[[nodiscard]] TaskId id() const { return _id; }

	void* esp() const { return _esp; }
	void* stack() const { return _stack; }

private:
	// start data
	task_routine* _routine{nullptr};
	void* _arg{nullptr};

	// os data
	const TaskId _id;
	lib::byte* _stack{};
	char _name[16]{"<unknown>"};

	// state
	void* _esp{nullptr};

	friend class Scheduler;
};

lib::ostream& operator<<(lib::ostream& out, const Task& task);

} // namespace x86
