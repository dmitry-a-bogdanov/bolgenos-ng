#include <bolgenos-ng/slab.h>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/memory.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/printk.h>

#include "config.h"


int get_status(const struct slab_area *slab, size_t index) {
	return slab->allocation_map[index];
}


void set_status(struct slab_area *slab, size_t index, int status) {
	slab->allocation_map[index] = status;
}


slab_status_t slab_init(struct slab_area *slab, size_t elem_size,
			size_t nelems) {
	slab->elem_size = elem_size;
	slab->nelems = nelems;
	size_t required_memory = elem_size * nelems + nelems;
	size_t required_pages =
		align_up(required_memory, PAGE_SIZE) / PAGE_SIZE;
	void *area = alloc_pages(required_pages);
	if (!area) {
		return SLAB_ENOMEM;
	}
	slab->allocation_map = area;
	slab->memory = ((uint8_t *) slab->allocation_map) + nelems;
	memset(slab->allocation_map, MEM_FREE, slab->nelems);
	/*
	for (size_t chunk = 0; chunk != slab->nelems; ++chunk) {
		printk("Checking chunk %lu:%lu\n", (long unsigned) chunk,
			(long unsigned) slab->allocation_map[chunk]);
	}
	*/
	return SLAB_OK;
}


void *slab_alloc(struct slab_area *slab) {
	if (!slab) {
		return NULL;
	}
	void *free_mem = NULL;
	for (size_t chunk = 0; chunk != slab->nelems; ++chunk) {
		if (slab->allocation_map[chunk] == MEM_FREE) {
			size_t offset = chunk * slab->elem_size;
			free_mem = (void *) (((size_t) (slab->memory)) +
				offset);
			slab->allocation_map[chunk] = MEM_USED;
			return free_mem;
		}
	}
	return free_mem;
}


void slab_free(struct slab_area *slab, void *entry) {
	if (!slab || !entry) {
		return;
	}
	size_t chunk = (((size_t) entry) - ((size_t) slab->memory)) /
		slab->elem_size;
	slab->allocation_map[chunk] = MEM_FREE;
}
