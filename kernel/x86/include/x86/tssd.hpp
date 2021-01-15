#pragma once

#include <cstdint.hpp>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/mem_utils.hpp>

#include "segment_flags.hpp"

namespace x86 {
class Task;
}

namespace x86 {

struct [[gnu::packed]] TaskStateSegmentDescriptor
{
	constexpr TaskStateSegmentDescriptor() = default;

	constexpr TaskStateSegmentDescriptor(uint32_t base,
					     uint32_t limit,
					     bool busy,
					     ProtectionRing dpl,
					     Present present,
					     Granularity granularity)
		:
		limit_00_15_(shiftmask(limit, 0, 0xffff)),
		base_00_15_(shiftmask(base, 0, 0xffff)),
		base_16_23_(shiftmask(base, 16, 0xff)),
		_type{static_cast<uint8_t>(0b1001 | (busy ? 0b0010 : 0b000))},
		_const_4_12{0},
		dpl_{dpl},
		present_{present},
		limit_16_19_(shiftmask(limit, 16, 0xf)),
		avl_{Avl::null},
		_const_4_21{0},
		_const_4_22{0},
		granularity_{granularity},
		base_24_31_(shiftmask(base, 24, 0xff)) {}

	uint16_t limit_00_15_: 16;
	uint16_t base_00_15_: 16;
	uint8_t base_16_23_: 8;
	uint8_t _type: 4;
	uint8_t _const_4_12: 1;
	ProtectionRing dpl_: 2;
	Present present_: 1;
	uint8_t limit_16_19_: 4;
	Avl avl_: 1;
	uint8_t _const_4_21: 1;
	uint8_t _const_4_22: 1;
	Granularity granularity_: 1;
	uint8_t base_24_31_: 8;
};

TaskStateSegmentDescriptor kernel_task_descriptor(const x86::Task* task);

}
