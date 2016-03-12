#include <bolgenos-ng/slab.hpp>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>

#include <bolgenos-ng/memory.hpp>

#include "config.h"

using namespace memory;

memory::allocators::SlabAllocator::SlabAllocator(size_t elem_size,
		size_t nelems) {
	_elem_size = elem_size;
	_nelems = nelems;
	size_t required_memory = _elem_size * _nelems + _nelems;
	size_t required_pages =
		align_up<PAGE_SIZE>(required_memory) / PAGE_SIZE;
	void *area = alloc_pages(required_pages);
	if (!area) {
		_initialized = false;
		return;
	}
	_allocation_map = reinterpret_cast<bool *>(area);
	_memory = reinterpret_cast<uint8_t *>(_allocation_map) + nelems;

	for(size_t elem = 0; elem != _nelems; ++elem) {
		set_free(elem, true);
	}
	_initialized = true;
}

memory::allocators::SlabAllocator::~SlabAllocator() {
	free_pages(_allocation_map);
	_memory = nullptr;
}

void *memory::allocators::SlabAllocator::allocate() {
	void *free_mem = nullptr;
	for (size_t chunk = 0; chunk != this->_nelems; ++chunk) {
		if (is_free(chunk)) {
			size_t offset = chunk * this->_elem_size;
			free_mem = (void *) (((size_t) (this->_memory)) +
				offset);
			set_free(chunk, false);
			return free_mem;
		}
	}
	return free_mem;
}


void memory::allocators::SlabAllocator::deallocate(void *addr) {
	if (!addr) {
		return;
	}
	size_t chunk = (((size_t) addr) - ((size_t) this->_memory)) /
		this->_elem_size;
	set_free(chunk, true);
}

bool memory::allocators::SlabAllocator::is_free(size_t index) {
	return _allocation_map[index];
}


void memory::allocators::SlabAllocator::set_free(size_t index, bool status) {
	_allocation_map[index] = status;
}

