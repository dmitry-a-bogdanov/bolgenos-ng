#include <bolgenos-ng/slab.hpp>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>

#include <bolgenos-ng/memory.hpp>

#include "config.h"

using namespace memory;

memory::allocators::SlabAllocator::SlabAllocator(size_t elem_size,
		size_t nelems) {
	elem_size_ = elem_size;
	nelems_ = nelems;
	size_t required_memory = elem_size_ * nelems_
			+ util::inplace::BitArray::expected_size(nelems_);
	size_t required_pages =
		align_up<PAGE_SIZE>(required_memory) / PAGE_SIZE;
	area_ = alloc_pages(required_pages);
	if (!area_) {
		initialized_ = false;
		return;
	}
	// _allocation_map = reinterpret_cast<bool *>(area);
	allocation_map_.initialize(area_, nelems_);
	memory_ = reinterpret_cast<char *>(area_)
			+ util::inplace::BitArray::expected_size(nelems_);

	for(size_t elem = 0; elem != nelems_; ++elem) {
		set_free(elem, true);
	}
	initialized_ = true;
}

memory::allocators::SlabAllocator::~SlabAllocator() {
	free_pages(area_);
	memory_ = nullptr;
}

void *memory::allocators::SlabAllocator::allocate() {
	void *free_mem = nullptr;
	for (size_t chunk = 0; chunk != nelems_; ++chunk) {
		if (is_free(chunk)) {
			size_t offset = chunk * elem_size_;
			free_mem = (void *) (((size_t) memory_) +
				offset);
			set_free(chunk, false);
			return free_mem;
		}
	}
	return free_mem;
}


bool memory::allocators::SlabAllocator::initialized() const {
	return initialized_;
}


void memory::allocators::SlabAllocator::deallocate(void *addr) {
	if (!addr) {
		return;
	}
	size_t chunk = (((size_t) addr) - ((size_t) memory_)) / elem_size_;
	set_free(chunk, true);
}

bool memory::allocators::SlabAllocator::is_free(size_t index) {
	return allocation_map_.get(index);
}


void memory::allocators::SlabAllocator::set_free(size_t index, bool status) {
	allocation_map_.set(index, status);
}

