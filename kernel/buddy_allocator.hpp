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
inline lib::ostream& operator <<(lib::ostream &stream,
		const pblk_t &blk) {
	return stream << "pblk_t {" << blk.ptr << ", " << blk.size << "}";
}


/// \brief Buddy system.
///
/// The class provides functionality of buddy system for the specified
/// memory region.
class BuddyAllocator {
public:

	/// Structure that hold statistics of allocator's usage.
	struct stats_type {
		/// Total number of allocations.
		size_t allocations = 0;


		/// Total number of deallocations.
		size_t deallocations = 0;
	};


	/// Statistic of allocator.
	stats_type stats = {};


	/// Constructor.
	BuddyAllocator() = default;


	/// Copy-constructing is denied
	BuddyAllocator(const BuddyAllocator&) = delete;


	/// Copy-assignment is denied
	BuddyAllocator& operator =(const BuddyAllocator&) = delete;


	/// \brief Initialize.
	///
	/// Initialize buddy allocator for specified memory region.
	/// \param region Memory region to be used in buddy system.
	/// \param max_order Max order of the free list allocator in
	/// the buddy system.
	void initialize(memory::MemoryRegion *region, size_t max_order);


	/// Destructor.
	virtual ~BuddyAllocator();


	/// \brief Put block to the system.
	///
	/// The functions puts specified page block to the buddy system.
	/// \param blk Page block to be put into the system.
	void put(pblk_t blk);


	/// \brief Get page block.
	///
	/// The function returns page block of specified size from the
	/// buddy system.
	///
	/// \param pages Number of pages in block to be allocated.
	/// \return Allocated page block. In case of error address of the block
	/// will be equal to nullptr.
	pblk_t get(size_t pages);


	/// Get memory region that is used for the buddy system.
	memory::MemoryRegion *region() const;


private:

	/// \brief Compute order of free list for page block.
	///
	/// The function computes order of free list for putting part of
	/// the specified page blocks.
	///
	/// \param blk Page block.
	/// \return order of the free list allocator.
	size_t compute_order(const pblk_t &blk);

	/// Set of free list allocators.
	FreeList *free_list_  = nullptr;


	/// Region of memory that are covered by this buddy system.
	memory::MemoryRegion *region_ = nullptr;


	/// Max order of freelist in buddy system.
	size_t max_order_ = 0;
}; // class BuddyAllocator


} // namespace allocators


} // namespace memory

