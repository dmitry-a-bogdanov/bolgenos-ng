#pragma once


#include <bolgenos-ng/memory_region.hpp>
#include <bolgenos-ng/page.hpp>

#include "free_list.hpp"


namespace memory {
namespace allocators {

struct pblk_t {
	page_frame_t *first = nullptr;
	size_t pages = 0;
};

inline cio::OutStream& operator <<(cio::OutStream &stream,
		const pblk_t &blk) {
	return stream << "pblk_t{" << blk.first << ", " << blk.pages << "}";
}


template<size_t MaxOrder>
class BuddyAllocator {
public:
	struct max_order {
		enum { value = MaxOrder };
	};
/*
	struct stats_type {
		size_t allocations = 0;
		size_t deallocations = 0;
		typename FreeList<max_order::value>::stats_type
				*list[max_order::value + 1];
	};
	stats_type stats;
*/

	BuddyAllocator() {
		region_ = nullptr;
		for(size_t i = 0; i <= max_order::value; ++i) {
			free_list_[i].initialize(i);
		}
	}

	void initialize(MemoryRegion *region) {
		region_ = region;
/*
		for(size_t i = 0; i <= max_order::value; ++i) {
			stats.list[i] = &free_list_[i].stats;
		}
*/
	}

	virtual ~BuddyAllocator(){}


	void put(pblk_t blk) {
		if (blk.pages == 0) {
			return;
		}
/*
		if (reinterpret_cast<size_t>(blk.first) & ((1 << 12) - 1)) {
			cio::cout << __func__ << ": bad page address: " << blk.first << cio::endl;
			panic(__func__);
		}
*/
/*
		++stats.deallocations;
*/

		while(blk.pages) {
			size_t block_order = compute_order(blk);
/*
			if (reinterpret_cast<size_t>(blk.first) & ((1 << 12) - 1)) {
						panic("bad page addr in while!");
			}
*/
			page_frame_t *squashed_page = free_list_[block_order].put(blk.first);
			if (squashed_page) {
				size_t putting_list = block_order + 1;
				while ((squashed_page =
						free_list_[putting_list].put(
							squashed_page))) {
					++putting_list;
				}
			}
			size_t block_size = 1 << block_order;
			blk.pages -= block_size;
			blk.first += block_size;
		}
	}


	pblk_t get(size_t pages) {
		pblk_t blk;
		blk.first = nullptr;
		blk.pages = 0;
		size_t order = 0;
/*
		++stats.allocations;
*/
		while (pages > (size_t(1) << order)) {
			++order;
		}

		if (order > max_order::value) {
			return blk;
		}


		while (order <= max_order::value) {

			blk.first = free_list_[order].get();
			if (blk.first)
				break;
			else
				++order;
		}

		if (!blk.first) {
			return blk;
		}

		blk.pages = pages;

		pblk_t extra_memory;
		extra_memory.first = blk.first + pages;
		extra_memory.pages = (1 << order) - pages;

		if (extra_memory.pages) {
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

		auto numeric_address = reinterpret_cast<size_t>(blk.first);
		auto page_number = numeric_address / PAGE_SIZE;
		while (order < max_order::value) {
			if (page_number & 0x1) {
				break;
			} else {
				++order;
				page_number = page_number / 2;
			}
		}

		if (blk.pages >= (size_t(1) << order)) {
			return order;
		}

		// Number of exiting pages is less than it's needed because of
		// computed order. Therefore we should decrease this number.

		while ((size_t(1) << order) > blk.pages) {
			--order;
		}

		return order;
	}

	FreeList<max_order::value> free_list_[max_order::value + 1];
	memory::MemoryRegion *region_;
}; // class BuddyAllocator


} // namespace allocators
} // namespace memory

