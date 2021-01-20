#pragma once

#include <atomic.hpp>
#include <cstddef.hpp>
#include <ext/intrusive_circular_list.hpp>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/loggable.hpp>

namespace lib {
class ostream;
}

namespace sched {

using task_routine = void (void *);

class Scheduler;

enum class TaskId: uint32_t {};

lib::ostream& operator<<(lib::ostream& out, TaskId id);

struct Task: private lib::Loggable {
public:
	Task() = delete;

	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

	~Task();

	void run();

	void name(const char* name);

	[[nodiscard]]
	const char* name() const { return _name; }

	[[gnu::thiscall]]
	static void wrapperForRun(Task* task);

	[[gnu::naked]]
	static void start_on_new_frame();

	[[nodiscard]]
	TaskId id() const { return _id; }

	[[nodiscard]]
	void* esp() const { return _esp; }

	[[nodiscard]]
	void* stack() const { return _stack; }

	[[nodiscard]]
	bool finished() const { return _exited.load(); }

	constexpr
	lib::IntrusiveListNode<Task>* tasks_list_node() { return &_tasks_list_node; }

protected:
	Task(Scheduler* creator, task_routine* routine, void* arg, const char *name = nullptr);

private:
	// start data
	task_routine* _routine{nullptr};
	void* _arg{nullptr};

	// os data
	const TaskId _id;
	lib::atomic<bool> _exited{false};
	lib::byte* _stack{};
	char _name[16]{"<unknown>"};

	lib::IntrusiveListNode<Task> _tasks_list_node{this};

	Scheduler* _scheduler;

	// state
	void* _esp{nullptr};

	friend class Scheduler;
};

lib::ostream& operator<<(lib::ostream& out, const Task& task);

} // namespace sched

namespace lib {
class ostream;
}
