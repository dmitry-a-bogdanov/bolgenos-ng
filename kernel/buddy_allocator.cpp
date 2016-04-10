#include "buddy_allocator.hpp"


#include <bolgenos-ng/memory.hpp>


memory::allocators::BuddyAllocator::~BuddyAllocator() {
}


void memory::allocators::BuddyAllocator::initialize(
		memory::MemoryRegion *region, size_t max_order) {
	region_ = region;
	max_order_ = max_order;
	free_list_ = nullptr;
}


void memory::allocators::BuddyAllocator::put(pblk_t blk) {
	if (free_list_ == nullptr) {
		size_t bytes_for_fls = sizeof(FreeList) * max_order_;
		size_t pages_for_fls = align_up<PAGE_SIZE>(
				bytes_for_fls) / PAGE_SIZE;
		if (pages_for_fls > blk.size) {
			panic("Not enough for BuddyAllocator");
		}

		free_list_ = reinterpret_cast<FreeList *>(blk.ptr);

		blk.ptr += pages_for_fls;
		blk.size -= pages_for_fls;

		for(size_t i = 0; i <= max_order_; ++i) {
			new (&free_list_[i]) FreeList;
			free_list_[i].initialize(i, i == max_order_);
		}
	}
	if (blk.size == 0) {
		return;
	}

	if (reinterpret_cast<size_t>(blk.ptr) & ((1 << 12) - 1)) {
		lib::cout	<< __func__ << ": bad page address: " << blk.ptr
				<< lib::endl;
		panic(__func__);
	}


	++stats.deallocations;


	while(blk.size) {
		size_t block_order = compute_order(blk);

		if (reinterpret_cast<size_t>(blk.ptr)
				& ((1 << 12) - 1)) {
			panic("bad page addr in while!");
		}

		page_frame_t *squashed_pages
			= free_list_[block_order].put(blk.ptr);
		if (squashed_pages) {
			size_t putting_list = block_order + 1;
			while ((squashed_pages =
					free_list_[putting_list].put(
						squashed_pages))) {
				++putting_list;
			}
		}
		size_t block_size = 1 << block_order;
		blk.size -= block_size;
		blk.ptr += block_size;
	}
}


memory::allocators::pblk_t memory::allocators::BuddyAllocator::get(size_t pages) {
	pblk_t blk = {nullptr, 0};
	size_t order = 0;

	++stats.allocations;

	while (pages > (size_t(1) << order)) {
		++order;
	}

	if (order > max_order_) {
		return blk;
	}

	while (order <= max_order_) {
		blk.ptr = free_list_[order].get();
		if (blk.ptr)
			break;
		else
			++order;
	}

	if (!blk.ptr) {
		return blk;
	}

	blk.size = pages;

	pblk_t extra_memory = {blk.ptr + pages, (1 << order) - pages};

	if (extra_memory.size) {
		put(extra_memory);
	}

	return blk;
}


memory::MemoryRegion *memory::allocators::BuddyAllocator::region() const {
	return region_;
}


size_t memory::allocators::BuddyAllocator::compute_order(const pblk_t &blk) {
	size_t order = 0;

	auto numeric_address = reinterpret_cast<size_t>(blk.ptr);
	auto page_number = numeric_address / PAGE_SIZE;
	while (((page_number & 0x1) == 0x0)
			&& order < max_order_) {
		++order;
		page_number >>= 1;
	}

	// Number of exiting pages is less than it's needed because of
	// computed order. Therefore we should decrease this number.
	while ((size_t(1) << order) > blk.size) {
		--order;
	}

	return order;
}
