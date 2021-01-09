#pragma once

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.hpp>
#include <x86/multitasking.hpp>

#include <x86/tssd.hpp>
#include <x86/memory_segment_d.hpp>

namespace mmu {



/// Size of segment descriptor.
constexpr int SEGMENT_STRUCT_SIZE = sizeof(MemorySegmentDescriptor);

enum SegmentIndex: int
{
	null = 0,
	kernel_code = 1,
	kernel_data = 2,
	kernel_scheduler = 3,
	first_task_index = kernel_scheduler,
	kernel_other_task = 4,
	kernel_other_task2 = 5,
	last_task_index = first_task_index + x86::TASKS,

	last_segment_idx = last_task_index,
	number_of_segments = last_segment_idx + 1,
};

union GDTEntry
{
	MemorySegmentDescriptor memory_segment;
	TaskStateSegmentDescriptor task;
};


/// Kernel code segment.
constexpr int KERNEL_CODE_SEGMENT_POINTER = SegmentIndex::kernel_code * SEGMENT_STRUCT_SIZE;

/// Kernel data segment.
constexpr int KERNEL_DATA_SEGMENT_POINTER = SegmentIndex::kernel_data * SEGMENT_STRUCT_SIZE;


/// \brief Setup segmentation support.
///
/// Setup segmentation support. The function initializes Global Descriptior
/// Table and loads it to appropriate CPU register.
void init();


} // namespace mmu

extern x86::TaskStateSegment other_task;
extern x86::TaskStateSegment other_task2;
