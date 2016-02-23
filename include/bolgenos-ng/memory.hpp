#pragma once

#include <bolgenos-ng/int_types.h>

namespace memory {
/**
* \brief Aling value down.
*
* Align given value down to specified boundary.
*
* \param value Value to be aligned.
* \param boundary Alignment boundary.
* \return Aligned value.
*/
static inline size_t align_down(size_t value, size_t boundary) {
	return value & ~(boundary - 1);
}


/**
* \brief Aling specified value.
*
* Align given value up to specified boundary.
*
* \param value Value to be aligned.
* \param boundary Alignment boundary.
* \return Aligned value.
*/
static inline size_t align_up(size_t value, size_t boundary) {
	return (value & (boundary - 1)) ?
		align_down(value, boundary) + boundary :
		value;
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


/*
* \brief Free allocated pages.
*
* The function frees allocated page block.
*
* \param addr Address of page block.
*/
void free_pages(void *addr);


} // namespace memory
