#pragma once

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.hpp>
#include <x86/multitasking.hpp>

#include <x86/tssd.hpp>
#include <x86/memory_segment_d.hpp>

namespace mmu {

enum SegmentIndex: int
{
	null = 0,
	kernel_code = 1,
	kernel_data = 2,
};


/// Kernel code segment.
constexpr uint16_t KERNEL_CODE_SEGMENT_SELECTOR =
	segment_selector(kernel_code, TableIndicator::GLOBAL, ring_kernel);

/// Kernel data segment.
constexpr uint16_t KERNEL_DATA_SEGMENT_SELECTOR =
	segment_selector(kernel_data, TableIndicator::GLOBAL, ring_kernel);

} // namespace mmu
