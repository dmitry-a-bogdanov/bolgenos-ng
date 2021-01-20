#pragma once

#include <ext/fixed_size_vector.hpp>

#include "memory_segment_d.hpp"
#include "tssd.hpp"

namespace x86 {

void reload_segment_registers();
void init_task_register();


enum class SegmentIndex: int
{
	null = 0,
	kernel_code = 1,
	kernel_data = 2,
	single_task = 3,
};

constexpr inline
uint16_t segment_selector(SegmentIndex index, TableIndicator ti,
			  ProtectionRing required_privilege_level) noexcept
{
	return segment_selector(static_cast<uint16_t>(index), ti, required_privilege_level);
}

/// Kernel code segment.
constexpr uint16_t KERNEL_CODE_SELECTOR =
	segment_selector(SegmentIndex::kernel_code, TableIndicator::GLOBAL, ProtectionRing::kernel);

/// Kernel data segment.
constexpr uint16_t KERNEL_DATA_SELECTOR =
	segment_selector(SegmentIndex::kernel_data, TableIndicator::GLOBAL, ProtectionRing::kernel);


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
