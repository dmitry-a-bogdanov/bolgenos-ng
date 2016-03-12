#include "page_allocator.hpp"

#include <bolgenos-ng/memory_region.hpp>

#include "buddy_allocator.hpp"

memory::allocators::PageAllocator pa;

namespace {


void *zero_size_page = reinterpret_cast<void *>(0x10);


} // namespace


void memory::allocators::PageAllocator::initialize(
		BuddyAllocator<buddy_order::value> *primary,
		page_frame_t *first_free) {
	primary_ = primary;
	auto region = primary_->region();

	auto map_size = memory::align_up<PAGE_SIZE>(
		util::inplace::BitArray::expected_size(region->size()))
				/ PAGE_SIZE;
	pblk_t pages;
	pages.ptr = first_free + map_size;
	pages.size = region->end() - pages.ptr;
	primary_->put(pages);
	map_.initialize(first_free, region->size());
}

void *memory::allocators::PageAllocator::allocate(size_t pages) {
	if (!pages) {
		// zero-size allocation should return valid address!
		return zero_size_page;
	}
	pblk_t free_memory = primary_->get(pages);
	if (!free_memory.ptr) {
		return nullptr;
	}

	size_t page_index = primary_->region()->index_of(free_memory.ptr);
	size_t last_page_in_block = page_index + free_memory.size - 1;

	map_.set(last_page_in_block, true);

	return free_memory.ptr;
}

void memory::allocators::PageAllocator::deallocate(void *memory) {
	if ((memory == nullptr) || (memory == zero_size_page)) {
		return;
	}

	auto frame = reinterpret_cast<memory::page_frame_t *>(memory);
	auto page_index = primary_->region()->index_of(frame);

	pblk_t blk = {frame, 0};

	for(size_t index = page_index; index != primary_->region()->size();
			++index) {
		++blk.size;
		if (map_.get(index)) {
			map_.set(index, false);
			break;
		}
	}

	primary_->put(blk);
}
