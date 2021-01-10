#pragma once

#include <ext/fixed_size_vector.hpp>

#include "multitasking.hpp"

#include "memory_segment_d.hpp"
#include "tssd.hpp"

namespace x86 {

// FIXME: remove
using namespace mmu;

[[gnu::noinline]] void reload_segment_registers();

class GDT {
public:
	GDT();

	union Entry
	{
		MemorySegmentDescriptor memory_segment;
		TaskStateSegmentDescriptor task;
	};

	void reload_table();

	uint16_t push_back(const MemorySegmentDescriptor& msd);
	uint16_t push_back(const TaskStateSegmentDescriptor& tssd);

private:

	alignas(cpu_alignment) lib::FixedSizeVector<Entry, 1024> _gdt{};
	alignas(cpu_alignment) table_pointer _gdt_pointer{};
};

}
