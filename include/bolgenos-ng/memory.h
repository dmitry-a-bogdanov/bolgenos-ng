#ifndef __BOLGENOS_NG__MEMORY_H__
#define __BOLGENOS_NG__MEMORY_H__

#include <bolgenos-ng/int_types.h>

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

#endif // __BOLGENOS_NG__MEMORY_H__
