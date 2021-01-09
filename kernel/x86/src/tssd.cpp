#include <x86/tssd.hpp>

#include <type_traits.hpp>

using namespace mmu;

static_assert(sizeof(TaskStateSegmentDescriptor) == 8, "Wrong size");

static_assert(lib::is_standard_layout_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not std layout");

static_assert(lib::is_trivial_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not trivial");

static_assert(lib::is_pod_v<TaskStateSegmentDescriptor>,
	      "TaskStateSegmentDescriptor is not POD");

