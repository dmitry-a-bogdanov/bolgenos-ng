#pragma once

#include <bolgenos-ng/int_types.h>

#include <bolgenos-ng/memory_region.hpp>

/// \brief Memory namespace
///
/// Namespace contains functionality that is related to high-level memory
/// management.
namespace memory {


/// \brief Aling specified value down.
///
/// Align given value down to specified boundary.
///
/// \tparam boundary Alignment boundary.
/// \tparam ValueType Type of value that is to be aligned.
/// \param value Value to be aligned.
/// \return Aligned value.
template<size_t boundary, typename ValueType>
inline ValueType align_down(ValueType value) {
	size_t _val = reinterpret_cast<size_t>(value);
	_val &= ~(boundary - 1);
	return reinterpret_cast<ValueType>(_val);
}


/// \brief Aling specified value up.
///
/// Align given value up to specified boundary.
///
/// \tparam boundary Alignment boundary.
/// \tparam ValueType Type of value that is to be aligned.
/// \param value Value to be aligned.
/// \return Aligned value.
template<size_t boundary, typename ValueType>
inline ValueType align_up(ValueType value) {
	size_t _val = reinterpret_cast<size_t>(value);
	if (_val & (boundary - 1)) {
		_val = align_down<boundary>(_val) + boundary;
	}
	return reinterpret_cast<ValueType>(_val);
}



/// \brief Initialize memory subsystem.
///
/// The function initializes memory subsystem.
void init();


/// \brief Allocate free pages.
///
/// The function allocates specified number of continious free pages if possible.
///
/// \param n Number of pages to be allocated.
/// \return Pointer to allocated area.
void *alloc_pages(size_t n);


/// \brief Free allocated pages.
///
/// The function frees allocated page block.
///
/// \param addr Address of page block.
void free_pages(void *addr);


/// \brief High memory.
///
/// High memory region descriptor.
extern MemoryRegion highmem;


} // namespace memory
