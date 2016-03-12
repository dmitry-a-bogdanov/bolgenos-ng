#pragma once

#include <bolgenos-ng/memory_region.hpp>
#include <bolgenos-ng/stdtypes.hpp>


/// \brief Memory namespace
///
/// Namespace contains functionality that is related to high-level memory
/// management.
namespace memory {
/**
* \brief Aling specified value down.
*
* Align given value down to specified boundary.
*
* \tparam Boundary Alignment boundary.
* \tparam ValueType Type of value that is to be aligned.
* \param value Value to be aligned.
* \return Aligned value.
*/
template<size_t Boundary, typename ValueType>
inline ValueType align_down(ValueType value) {
	constexpr size_t alignment_mask = ~(Boundary - 1);
	auto _val = reinterpret_cast<size_t>(value);
	_val &= alignment_mask;
	return reinterpret_cast<ValueType>(_val);
}


/**
* \brief Aling specified value up.
*
* Align given value up to specified boundary.
*
* \tparam Boundary Alignment boundary.
* \tparam ValueType Type of value that is to be aligned.
* \param value Value to be aligned.
* \return Aligned value.
*/
template<size_t Boundary, typename ValueType>
inline ValueType align_up(ValueType value) {
	constexpr size_t alignment_mask = Boundary - 1;
	auto _val = reinterpret_cast<size_t>(value);
	if (_val & alignment_mask) {
		_val = align_down<Boundary>(_val) + Boundary;
	}
	return reinterpret_cast<ValueType>(_val);
}


/**
* \brief Initialize memory subsystem.
*
* The function initializes memory subsystem.
*/
void init();


/**
* \brief Allocate free pages.
*
* The function allocates specified number of continious free pages if possible.
*
* \param n Number of pages to be allocated.
*
* \return Pointer to allocated area.
*/
void *alloc_pages(size_t n);


/**
* \brief Free allocated pages.
*
* The function frees allocated page block.
*
* \param addr Address of page block.
*/
void free_pages(void *addr);


} // namespace memory
