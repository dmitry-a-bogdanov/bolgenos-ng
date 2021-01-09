#pragma once

#include <cstdint.hpp>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/mem_utils.hpp>

#include "segment_flags.hpp"

namespace mmu {


/// \brief MemorySegmentDescriptor descriptor.
///
/// Structure that provides fine interface for filling segment descriptor.
struct _packed_ MemorySegmentDescriptor
{
public:
	constexpr MemorySegmentDescriptor() = default;

	constexpr MemorySegmentDescriptor(uint32_t base,
					  uint32_t limit,
					  tag_type tag,
					  seg_sys_flag_type ssf,
					  protection_ring_t dpl,
					  seg_present_type present,
					  seg_long_type lng,
					  seg_db_type db,
					  seg_granularity_type granularity)
		:
		limit_00_15_(bitmask(limit, 0, 0xffff)),
		base_00_15_(bitmask(base, 0, 0xffff)),
		base_16_23_(bitmask(base, 16, 0xff)),
		tag_(tag),
		system_flag_(ssf),
		dpl_(dpl),
		present_(present),
		limit_16_19_(bitmask(limit, 16, 0xf)),
		avl_(seg_avl_type::avl_null),
		long_(lng),
		db_(db),
		granularity_(granularity),
		base_24_31_(bitmask(base, 24, 0xff))
	{
	}

private:
	uint16_t limit_00_15_: 16;
	uint16_t base_00_15_: 16;
	uint8_t base_16_23_: 8;
	tag_type tag_: 4;
	seg_sys_flag_type system_flag_: 1;
	protection_ring_t dpl_: 2;
	seg_present_type present_: 1;
	uint8_t limit_16_19_: 4;
	seg_avl_type avl_: 1;
	seg_long_type long_: 1;
	seg_db_type db_: 1;
	seg_granularity_type granularity_: 1;
	uint8_t base_24_31_: 8;
};


}
