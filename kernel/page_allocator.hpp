#pragma once

#include <bolgenos-ng/bitarray.hpp>


namespace memory {


namespace allocators {


class BuddyAllocator; // forward declaration
class MemoryRegion; // forward declaration


/// \brief Page allocator.
///
/// The function provied functionality for by-page allocation on top of
/// the buddy system.
class PageAllocator {
public:
	struct buddy_order {
		/// Maximal order of the free list in the buddy system.
		constexpr static size_t value = 10;
	};


	/// Default constructor.
	PageAllocator() = default;


	/// Copy-initialization is denied.
	PageAllocator(const PageAllocator &) = delete;

	/// Copy-assignment is denied.
	PageAllocator& operator =(const PageAllocator &) = delete;


	/// Destrutor.
	~PageAllocator() {}


	/// \brief Initialize page allocator.
	///
	/// The functions initializes page allocator on top of the specified
	/// buddy system with assumtion that part memory is free starting
	/// with specified address.
	///
	/// \param primary Pointer to the buddy system.
	/// \param first_free Address of the beginning of free memory.
	void initialize(BuddyAllocator *primary, page_frame_t *first_free);


	/// \brief Allocate pages.
	///
	/// The function allocates specified number of pages.
	///
	/// \param Number of pages to allocate.
	/// \return Pointer to allocated memory.
	void *allocate(size_t pages);


	/// \brief Deallocate pages.
	///
	/// The function releases previosly allocated pages.
	///
	/// \param Pointer to previously allocated memory.
	void deallocate(void *memory);

private:
	/// Pointer to the buddy system.
	BuddyAllocator *primary_ = nullptr;


	/// map for keeping ends of page blocks.
	util::inplace::BitArray map_ = {};
};


} // namespace allocators

} // namespace memory
