#include <x86/tssd.hpp>

#include <type_traits.hpp>

#include <sched/task.hpp>

using namespace x86;
using namespace lib;

static_assert(sizeof(TaskStateSegmentDescriptor) == 8, "Wrong size");

static_assert(lib::is_standard_layout_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not std layout");

static_assert(lib::is_trivial_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not trivial");

static_assert(lib::is_pod_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not POD");

TaskStateSegmentDescriptor x86::kernel_task_descriptor(const sched::Task* task)
{
	return {
		reinterpret_cast<uintptr_t>(task),
		sizeof(sched::Task) - 1,
		false,
		ProtectionRing::kernel,
		Present::present,
		Granularity::bytes
	};
}
