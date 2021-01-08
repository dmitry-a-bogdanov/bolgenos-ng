#pragma once

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/x86/multitasking.hpp>

namespace mmu {


/// \brief Memory segment: system flag
///
/// Enum holds values of system flag for memory segment descriptor.
enum seg_sys_flag_type
{
	/// NULL-segment
	sys_null = 0x0,


	/// System segment
	system = 0x0,


	/// Code or data segments
	code_or_data = 0x1,
};


/**
* \brief Memory segment: present flag.
*
* Enum holds values of present flag for memory segment descriptor.
*/
enum seg_present_type
{
	present_null = 0x0, /*!< NULL-segment */
	not_present = 0x0, /*!< Segment is not present */
	present = 0x1, /*!< Segment is present */
};


/**
* \brief Memory segment: long flag.
*
* Enum holds values of long flag for memory segment descriptor.
*/
enum seg_long_type
{
	long_null = 0x0, /*!< NULL-segment */
	long_64 = 0x1, /*!< 64-bit segment */
	other = 0x0, /*!< 32-bit segment */
};


enum seg_db_type
{
	db_null = 0x0,
	db32_bit = 0x1,
	db16_bit = 0x0,
};


/**
* \brief Memory segment: granularity flag.
*
* Enum holds values of granularity flag for memory segment descriptor.
*/
enum seg_granularity_type
{
	granularity_null = 0x0, /*!< NULL-segment */
	four_k_pages = 0x1, /*!< Use 4K pages */
	bytes = 0x0, /*!< Do not use pages */
};


enum seg_avl_type
{
	avl_null = 0x0
};

/// \brief Memory segment: D/B flag.
///
/// \warning hard to document this option without good description of
/// segmentation mechanism.
///
/// TODO: document this shit.
enum tag_type
{
	st_null = 0 << 0,
	// common
	accessed = 1 << 0,

	// data & data-specific flags
	data = 0 << 3,
	data_write = 1 << 1,
	data_down = 1 << 2,

	// code & code-specific flags
	code = 1 << 3,
	code_read = 1 << 1,
	code_conforming = 1 << 2,
};


/// \brief Segment descriptor.
///
/// Structure that provides fine interface for filling segment descriptor.
struct _packed_ Segment
{
public:
	constexpr Segment() = default;
	constexpr Segment(uint32_t base,
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
	uint16_t		limit_00_15_: 16;
	uint16_t		base_00_15_: 16;
	uint8_t			base_16_23_: 8;
	tag_type		tag_: 4;
	seg_sys_flag_type	system_flag_: 1;
	protection_ring_t	dpl_: 2;
	seg_present_type	present_: 1;
	uint8_t			limit_16_19_: 4;
	seg_avl_type		avl_: 1;
	seg_long_type		long_: 1;
	seg_db_type		db_: 1;
	seg_granularity_type	granularity_: 1;
	uint8_t			base_24_31_: 8;
};

struct _packed_ TaskStateSegmentDescriptor
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
		base_24_31_(bitmask(base, 24, 0xff))
	{}

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

/// Size of segment descriptor.
constexpr int SEGMENT_STRUCT_SIZE = sizeof(Segment);

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
	Segment memory_segment;
	TaskStateSegmentDescriptor task_descriptor;
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
