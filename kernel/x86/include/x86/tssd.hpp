#pragma once

#include <cstdint.hpp>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/mem_utils.hpp>

#include "segment_flags.hpp"

namespace x86 {
class Task;
}

namespace mmu {

struct [[gnu::packed]] TaskStateSegmentDescriptor
{
	constexpr TaskStateSegmentDescriptor() = default;

	constexpr TaskStateSegmentDescriptor(uint32_t base,
					     uint32_t limit,
					     bool busy,
					     protection_ring_t dpl,
					     seg_present_type present,
					     seg_granularity_type granularity)
		:
		limit_00_15_(bitmask(limit, 0, 0xffff)),
		base_00_15_(bitmask(base, 0, 0xffff)),
		base_16_23_(bitmask(base, 16, 0xff)),
		_type{static_cast<uint8_t>(0b1001 | (busy ? 0b0010 : 0b000))},
		_const_4_12{0},
		dpl_{dpl},
		present_{present},
		limit_16_19_(bitmask(limit, 16, 0xf)),
		avl_{seg_avl_type::avl_null},
		_const_4_21{0},
		_const_4_22{0},
		granularity_{granularity},
		base_24_31_(bitmask(base, 24, 0xff)) {}

	uint16_t limit_00_15_: 16;
	uint16_t base_00_15_: 16;
	uint8_t base_16_23_: 8;
	uint8_t _type: 4;
	uint8_t _const_4_12: 1;
	protection_ring_t dpl_: 2;
	seg_present_type present_: 1;
	uint8_t limit_16_19_: 4;
	seg_avl_type avl_: 1;
	uint8_t _const_4_21: 1;
	uint8_t _const_4_22: 1;
	seg_granularity_type granularity_: 1;
	uint8_t base_24_31_: 8;
};

TaskStateSegmentDescriptor kernel_task_descriptor(const x86::Task* task);

}
