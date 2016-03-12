#pragma once


#include <bolgenos-ng/memory_region.hpp>
#include <bolgenos-ng/page.hpp>

#include "free_list.hpp"


namespace memory {
namespace allocators {


struct pblk_t {
	pblk_t() = default;
	pblk_t(page_frame_t *p, size_t s)
		: ptr(p), size(s) {}
	page_frame_t *ptr = nullptr;
	size_t size = 0;
};

inline cio::OutStream& operator <<(cio::OutStream &stream,
		const pblk_t &blk) {
	return stream << "pblk_t{" << blk.ptr << ", " << blk.size << "}";
}


template<size_t MaxOrder>
class BuddyAllocator {
public:
	struct max_order {
		static constexpr size_t value = MaxOrder;
	};

	struct stats_type {
		size_t allocations = 0;
		size_t deallocations = 0;
		typename FreeList<max_order::value>::stats_type
				*list[max_order::value + 1];
	};
	stats_type stats;


	BuddyAllocator() {
		region_ = nullptr;
		for(size_t i = 0; i <= max_order::value; ++i) {
			free_list_[i].initialize(i);
		}
	}

	void initialize(MemoryRegion *region) {
		region_ = region;

		for(size_t i = 0; i <= max_order::value; ++i) {
			stats.list[i] = &free_list_[i].stats;
		}
	}

	virtual ~BuddyAllocator(){}


	void put(pblk_t blk) {
		if (blk.size == 0) {
			return;
		}

		if (reinterpret_cast<size_t>(blk.ptr) & ((1 << 12) - 1)) {
			cio::cout << __func__ << ": bad page address: " << blk.ptr << cio::endl;
			panic(__func__);
		}


		++stats.deallocations;


		while(blk.size) {
			size_t block_order = compute_order(blk);

			if (reinterpret_cast<size_t>(blk.ptr) & ((1 << 12) - 1)) {
						panic("bad page addr in while!");
			}

			page_frame_t *squashed_pages = free_list_[block_order].put(blk.ptr);
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


	pblk_t get(size_t pages) {
		pblk_t blk = {nullptr, blk.size};
		size_t order = 0;

		++stats.allocations;

		while (pages > (size_t(1) << order)) {
			++order;
		}

		if (order > max_order::value) {
			return blk;
		}

		while (order <= max_order::value) {

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


	inline memory::MemoryRegion *region() const {
		return region_;
	}

private:
	static size_t compute_order(const pblk_t &blk) {
		size_t order = 0;

		auto numeric_address = reinterpret_cast<size_t>(blk.ptr);
		auto page_number = numeric_address / PAGE_SIZE;
		while (((page_number & 0x1) == 0x0)
					&& order < max_order::value) {
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

	FreeList<max_order::value> free_list_[max_order::value + 1];
	memory::MemoryRegion *region_;
}; // class BuddyAllocator


} // namespace allocators
} // namespace memory

