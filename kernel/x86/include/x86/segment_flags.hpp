#pragma once

namespace mmu {


/// \brief Memory segment: system flag
///
/// Enum holds values of system flag for memory segment descriptor.
enum class seg_sys_flag_type
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
	not_present = 0x0, /*!< MemorySegmentDescriptor is not present */
	present = 0x1, /*!< MemorySegmentDescriptor is present */
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

enum class TableIndicator {
	GLOBAL = 0,
	LOCAL = 1
};


constexpr inline
uint16_t segment_selector(uint16_t segment_idx, TableIndicator ti,
			       protection_ring_t required_privilege_level) noexcept
{
	return (segment_idx << 3)
	       | (static_cast<uint16_t>(ti) << 2)
	       | (static_cast<uint16_t>(required_privilege_level));
}

}

