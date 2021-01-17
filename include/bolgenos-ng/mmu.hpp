#pragma once

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.hpp>

#include <x86/tssd.hpp>
#include <x86/memory_segment_d.hpp>

namespace x86 {

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

} // namespace x86
