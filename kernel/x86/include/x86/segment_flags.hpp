#pragma once

#include <type_traits.hpp>

namespace x86 {


/// \brief Memory segment: system flag
///
/// Enum holds values of system flag for memory segment descriptor.
enum class System
{
	/// NULL-segment
	null = 0x0,


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
enum class Present
{
	null = 0x0, /*!< NULL-segment */
	not_present = 0x0, /*!< MemorySegmentDescriptor is not present */
	present = 0x1, /*!< MemorySegmentDescriptor is present */
};


/**
* \brief Memory segment: long flag.
*
* Enum holds values of long flag for memory segment descriptor.
*/
enum class Long
{
	null = 0x0, /*!< NULL-segment */
	long_64 = 0x1, /*!< 64-bit segment */
	other = 0x0, /*!< 32-bit segment */
};


enum class OperationSize: uint8_t
{
	null = 0x0,
	db32_bit = 0x1,
	db16_bit = 0x0,
};


/**
* \brief Memory segment: granularity flag.
*
* Enum holds values of granularity flag for memory segment descriptor.
*/
enum class Granularity: uint8_t
{
	null = 0x0, /*!< NULL-segment */
	four_k_pages = 0x1, /*!< Use 4K pages */
	bytes = 0x0, /*!< Do not use pages */
};


enum class Avl
{
	null = 0x0
};

enum class SegmentType: uint8_t
{
	null = 0 << 0,
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

template<class T>
constexpr T enum_or(T lhs, T rhs) {
	using UT = lib::underlying_type_t<T>;
	return static_cast<T>(static_cast<UT>(lhs) | static_cast<UT>(rhs));
}

constexpr SegmentType operator|(SegmentType lhs, SegmentType rhs) {
	return enum_or(lhs, rhs);
}

enum class TableIndicator {
	GLOBAL = 0,
	LOCAL = 1
};


constexpr inline
uint16_t segment_selector(uint16_t segment_idx, TableIndicator ti,
			  ProtectionRing required_privilege_level) noexcept
{
	return (segment_idx << 3)
	       | (static_cast<uint16_t>(ti) << 2)
	       | (static_cast<uint16_t>(required_privilege_level));
}

}

