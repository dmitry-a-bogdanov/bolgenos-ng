#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>


namespace bolgenos {


bool is_initialized();


template<std::uintmax_t Boundary, typename T>
constexpr T align_down(T value)
{
	if constexpr (std::is_pointer_v<T>)
	{
		return align_down<Boundary, std::uintptr_t>(value);
	}
	else
	{
		using Type = std::make_unsigned_t<T>;
		Type mask = ~(static_cast<Type>(Boundary) - 1);
		Type retval = static_cast<Type>(value);
		retval &= mask;
		return static_cast<T>(retval);
	}
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

class MultibootInfo;
void init_memory(const MultibootInfo*);


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


/// \brief Allocate kernel memory.
///
/// The function allocates memory from kernel mallocator.
///
/// \param bytes Amount of memory to allocate.
void *kmalloc(size_t bytes);


/// \brief Release memory.
///
/// The function returns back to mallocator memory that was previously
/// allocated.
///
/// \param memory Pointer to previously allocated memory.
void kfree(void *memory);


} // namespace memory
