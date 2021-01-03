#include "mallocator.hpp"

#include <bolgenos-ng/error.h>

#include <lib/ostream.hpp>

namespace {

size_t chain_unit_size(size_t index) {
	if (index == 0)
		return 8;
	return index * 16;
}

} // namespace

void memory::allocators::Mallocator::initialize(
		memory::allocators::PageAllocator *fallback,
		size_t chain_memory, size_t chain_length) {
	fallback_ = fallback;
	chain_length_ = chain_length;
	const size_t chain_size = sizeof(SlabAllocator)*chain_length;
	if (! internal_allocator_.initialize(chain_size, 1)) {
		panic("Initializing of internal allocator failed!");
	}

	chain_ = static_cast<SlabAllocator *>(
			internal_allocator_.allocate());
	for(size_t slab_idx = 0; slab_idx != chain_length_; ++slab_idx) {
		new (static_cast<void *>(&chain_[slab_idx]))
				SlabAllocator;
		const size_t elem_size = chain_unit_size(slab_idx);
		if (!chain_[slab_idx].initialize(elem_size,
				chain_memory / elem_size)) {
			panic("Initializing of chain failed!");
		}
	}
}

void *memory::allocators::Mallocator::allocate(size_t bytes) {
	size_t slab_idx;
	if (bytes <= 8) {
		slab_idx = 0;
	} else {
		slab_idx = align_up<16>(bytes) / 16;
		if (slab_idx >= chain_length_) {
			slab_idx = chain_length_;
		}
	}

	while (slab_idx != chain_length_) {
		void *memory = nullptr;
		memory = chain_[slab_idx].allocate();
		if (memory) {
			return memory;
		}
		++slab_idx;
	}

	size_t pages = align_up<PAGE_SIZE>(bytes) / PAGE_SIZE;
	return fallback_->allocate(pages);
}


void memory::allocators::Mallocator::deallocate(void *memory) {
	size_t chain_idx = 0;
	while (chain_idx != chain_length_) {
		if (chain_[chain_idx].owns(memory)) {
			chain_[chain_idx].deallocate(memory);
			return;
		}
		++chain_idx;
	}
	fallback_->deallocate(memory);
}
