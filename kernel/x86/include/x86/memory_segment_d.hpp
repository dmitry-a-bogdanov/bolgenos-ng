#pragma once

#include <cstdint.hpp>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/mem_utils.hpp>

#include "segment_flags.hpp"

namespace x86 {


/// \brief MemorySegmentDescriptor descriptor.
///
/// Structure that provides fine interface for filling segment descriptor.
struct _packed_ MemorySegmentDescriptor
{
public:
	constexpr MemorySegmentDescriptor() = default;

	constexpr MemorySegmentDescriptor(uint32_t base,
					  uint32_t limit,
					  SegmentType tag,
					  System ssf,
					  ProtectionRing dpl,
					  Present present,
					  Long lng,
					  OperationSize db,
					  Granularity granularity)
		:
		limit_00_15_(bitmask(limit, 0, 0xffff)),
		base_00_15_(bitmask(base, 0, 0xffff)),
		base_16_23_(bitmask(base, 16, 0xff)),
		tag_(tag),
		system_flag_(ssf),
		dpl_(dpl),
		present_(present),
		limit_16_19_(bitmask(limit, 16, 0xf)),
		avl_(Avl::null),
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
	SegmentType tag_: 4;
	System system_flag_: 1;
	ProtectionRing dpl_: 2;
	Present present_: 1;
	uint8_t limit_16_19_: 4;
	Avl avl_: 1;
	Long long_: 1;
	OperationSize db_: 1;
	Granularity granularity_: 1;
	uint8_t base_24_31_: 8;
};


}
