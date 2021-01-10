#include <x86/tssd.hpp>

#include <type_traits.hpp>

#include <x86/multitasking.hpp>

using namespace mmu;
using namespace lib;

static_assert(sizeof(TaskStateSegmentDescriptor) == 8, "Wrong size");

static_assert(lib::is_standard_layout_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not std layout");

static_assert(lib::is_trivial_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not trivial");

static_assert(lib::is_pod_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not POD");

TaskStateSegmentDescriptor mmu::kernel_task_descriptor(const x86::Task* task)
{
	return {
		reinterpret_cast<uintptr_t>(task),
		sizeof(x86::Task) - 1,
		false,
		protection_ring_t::ring_kernel,
		seg_present_type::present,
		seg_granularity_type::bytes
	};
}
