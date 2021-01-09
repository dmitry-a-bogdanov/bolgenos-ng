#include <x86/memory_segment_d.hpp>

#include <type_traits.hpp>

using namespace mmu;

static_assert(lib::is_standard_layout_v<MemorySegmentDescriptor>,
	      "MemorySegmentDescriptor is not std layout");

static_assert(lib::is_trivial_v<MemorySegmentDescriptor>,
	      "MemorySegmentDescriptor is not trivial");

static_assert(lib::is_pod_v<MemorySegmentDescriptor>,
	      "MemorySegmentDescriptor is not POD");

static_assert(sizeof(MemorySegmentDescriptor) == 8, "MemorySegmentDescriptor has wrong size");
