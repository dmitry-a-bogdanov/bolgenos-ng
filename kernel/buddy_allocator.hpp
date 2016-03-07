#pragma once

#include <bolgenos-ng/utility>

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


template<size_t n>
struct power_of_2 {
	enum {
		value = 2 * power_of_2<n - 1>::value
	};
};


template<>
struct power_of_2<0> {
	enum {
		value = 1
	};
};

template<typename T>
T min(const T& a, const T& b) {
	if (a < b)
		return a;
	else
		return b;
}


template<size_t MaxOrder>
class BuddyAllocator {
public:
	struct max_order {
		enum { value = MaxOrder };
	};

	BuddyAllocator() {
		region_ = nullptr;
		for(size_t i = 0; i <= max_order::value; ++i) {
			free_list_[i].initialize(i);
		}
	}

	void initialize(MemoryRegion *region) {
		region_ = region;
	}

	virtual ~BuddyAllocator(){}


	void put(pblk_t blk) {
		if (blk.pages == 0) {
			return;
		}
		while(blk.pages) {
			size_t block_order = compute_order(blk);
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

private:
	static size_t compute_order(const pblk_t &blk) {
		size_t order = 0;

		auto numeric_address = reinterpret_cast<size_t>(blk.first);
		while (order < max_order::value) {
			if (order % 2) {
				break;
			} else {
				++order;
				numeric_address >>= 2;
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
	MemoryRegion *region_;
}; // class BuddyAllocator


} // namespace allocators
} // namespace memory

