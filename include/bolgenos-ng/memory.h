#ifndef __BOLGENOS_NG__MEMORY_H__
#define __BOLGENOS_NG__MEMORY_H__

#include <bolgenos-ng/int_types.h>


/**
* \brief Page is used flag.
*
* Flag for page::free field that the page is used.
*/
#define MEM_USED			(0x0)


/**
* \brief Page is free flag.
*
* Flag for page::free field that the page is free.
*/
#define MEM_FREE			(0x1)


/**
* \brief Aling value down.
*
* Align given value down to specified boundary.
*
* \param addr Value to be aligned.
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
* \param addr Value to be aligned.
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
void init_memory();


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

#endif // __BOLGENOS_NG__MEMORY_H__
