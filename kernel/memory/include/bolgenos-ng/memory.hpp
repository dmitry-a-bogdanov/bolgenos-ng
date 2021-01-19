#pragma once

#include <cstddef.hpp>
#include <cstdint.hpp>
#include <type_traits.hpp>
#include <bolgenos-ng/mem_utils.hpp>

namespace memory {

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
