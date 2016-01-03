#ifndef __BOLGENOS_NG__SLAB_H__
#define __BOLGENOS_NG__SLAB_H__

#include <bolgenos-ng/int_types.h>


/**
* \brief Slab allocation area descriptor.
*
* The structure holds info for slab allocator.
*/
struct slab_area {
	size_t elem_size;		/*!< Size of element in size slab. */
	size_t nelems;			/*!< Number of elements in slab. */
	void *memory;			/*!< Pointer to memory of slab. */
	uint8_t *allocation_map;	/*!< Allocation status of elements. */
};


/**
* \brief Slab initialization status.
*
* Enum is used as return value of \ref slab_init function.
*/
typedef enum {
	SLAB_OK = 0,		/*!< Success. */
	SLAB_ENOMEM,		/*!< Not enough memory is available. */
} slab_status_t;


/**
* \brief Initialize slab.
*
* The function allocates memory, setups allocation map and fills provided
* slab area descriptor.
*
* \param slab Slab area descriptor.
* \param elem_size Size of element in slab area.
* \param nelems Number of elemnts in slab area.
* \return Status of memory allocation.
*/
slab_status_t slab_init(struct slab_area *slab, size_t elem_size, size_t nelems);


/**
* \brief Allocate element from slab.
*
* The function allocates one element from slab if possible.
*
* \param slab Slab area descriptor.
* \return Pointer to allocated memory or NULL.
*/
void *slab_alloc(struct slab_area *slab);


/**
* \brief Free allocated element.
*
* The function frees specified element in the given slab area.
*
* \param slab Slab area descriptor.
* \param entry Allocated element from slab area.
*/
void slab_free(struct slab_area *slab, void *entry);

#endif // __BOLGENOS_NG__SLAB_H__
