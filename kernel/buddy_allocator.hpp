#pragma once


#include <bolgenos-ng/memory_region.hpp>
#include <bolgenos-ng/page.hpp>

#include "free_list.hpp"


namespace memory {


/// Allocator classes and functions.
namespace allocators {


/// \brief Page block.
///
/// Structure describes block of consequent pages.
struct pblk_t {
	///  Constructor.
	pblk_t() = default;


	/// Constructor that sets all fields.
	pblk_t(page_frame_t *p, size_t s)
		: ptr(p), size(s) {}


	/// Pointer to the first page frames.
	page_frame_t *ptr = nullptr;


	/// Size of page block in pages.
	size_t size = 0;
};


/// Output operator for \ref pblk_t
inline cio::OutStream& operator <<(cio::OutStream &stream,
		const pblk_t &blk) {
	return stream << "pblk_t {" << blk.ptr << ", " << blk.size << "}";
}


/// \brief Buddy system.
///
/// The class provides functionality of buddy system for the specified
/// memory region.
///
/// \tparam MaxOrder Maximal order of \ref FreeList in buddy system.
template<size_t MaxOrder>
class BuddyAllocator {
public:
	struct max_order {
		/// Compile-time constant that holds maximal order of
		/// \ref FreeList in buddy system.
		static constexpr size_t value = MaxOrder;
	};


	/// Structure that hold statistics of allocator's usage.
	struct stats_type {
		/// Total number of allocations.
		size_t allocations = 0;


		/// Total number of deallocations.
		size_t deallocations = 0;

		/// Array of pointers to stats structure of of \ref FreeList.
		FreeList::stats_type
				*list[max_order::value + 1];
	};


	/// Statistic of allocator.
	stats_type stats;


	/// Constructor.
	BuddyAllocator() {
		region_ = nullptr;
		for(size_t i = 0; i <= max_order::value; ++i) {
			free_list_[i].initialize(i, i == max_order::value);
		}
	}


	/// \brief Initialize.
	///
	/// Initialize buddy allocator for specified memory region.
	/// \param region Memory region to be used in buddy system.
	void initialize(MemoryRegion *region) {
		region_ = region;

		for(size_t i = 0; i <= max_order::value; ++i) {
			stats.list[i] = &free_list_[i].stats;
		}
	}


	/// Destructor.
	virtual ~BuddyAllocator(){}


	/// \brief Put block to the system.
	///
	/// The functions puts specified page block to the buddy system.
	/// \param blk Page block to be put into the system.
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


	/// \brief Get page block.
	///
	/// The function returns page block of specified size from the
	/// buddy system.
	///
	/// \param pages Number of pages in block to be allocated.
	/// \return Allocated page block. In case of error address of the block
	/// will be equal to nullptr.
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


	/// Get memory region that is used for the buddy system.
	inline memory::MemoryRegion *region() const {
		return region_;
	}


private:

	/// \brief Compute order of free list for page block.
	///
	/// The function computes order of free list for putting part of
	/// the specified page blocks.
	///
	/// \param blk Page block.
	/// \return order of the free list allocator.
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

	/// Set of free list allocators.
	FreeList free_list_[max_order::value + 1];


	/// Region of memory that are covered by this buddy system.
	memory::MemoryRegion *region_;
}; // class BuddyAllocator


} // namespace allocators


} // namespace memory

