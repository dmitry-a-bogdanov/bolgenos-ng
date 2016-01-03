#ifndef __BOLGENOS_NG__SLAB_H__
#define __BOLGENOS_NG__SLAB_H__

#include <bolgenos-ng/int_types.h>

struct slab_area {
	size_t elem_size;
	size_t nelems;
	void *memory;
	uint8_t *allocation_map;
};

typedef enum {
	SLAB_OK = 0,
	SLAB_ENOMEM,
} slab_status_t;

slab_status_t slab_init(struct slab_area *slab, size_t elem_size, size_t nelems);


void *slab_alloc(struct slab_area *slab);


void slab_free(struct slab_area *slab, void *entry);

#endif // __BOLGENOS_NG__SLAB_H__
