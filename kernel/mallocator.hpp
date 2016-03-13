#pragma once

#include <bolgenos-ng/slab.hpp>

#include "page_allocator.hpp"


namespace memory {


namespace allocators {


class Mallocator {
public:
	void initialize(PageAllocator *fallback,
			size_t chain_memory, size_t chain_length);
	void *allocate(size_t bytes);
	void deallocate(void *memory);
private:
	size_t chain_length_;
	/// Chain of slab allocators.
	SlabAllocator *chain_;
	PageAllocator *fallback_;
	SlabAllocator internal_allocator_;
};



} // namespace allocators


} // namespace memory
