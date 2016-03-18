#pragma once

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/page.hpp>
#include <bolgenos-ng/stdtypes.hpp>

#include "config.h"


namespace memory {


namespace allocators {


/// \brief Free list allocator.
///
/// The structure provides functionality of free list list allocator. Such
/// allocator keeps one-directional list of blocks of page frames.
class FreeList {
	struct item_type; // forward declaration.

public:

	/// Structure that holds statistics of the usage of the free list.
	struct stats_type {
		/// Number of elements in list.
		size_t items = 0;
	};


	/// Static of the usage of this free list.
	stats_type stats = {};


	/// Default constructor.
	FreeList() = default;


	/// Copy-initialization is denied.
	FreeList(const FreeList &) = delete;


	/// Copy-assignment is denied.
	FreeList& operator =(const FreeList &) = delete;


	/// Destructor.
	~FreeList() {}


	/// \brief Initialize.
	///
	/// The function initializes the free list structure of
	/// the specified order.
	///
	/// \param order Order of free list.
	/// \param disable_squashing Boolean flag shows whether should
	/// page squashing be disabled or not.
	/// \return boolean status of initialization. true if success; false
	/// otherwise.
	bool initialize(size_t order, bool disable_squashing = false);


	/// \brief Get next free page block.
	///
	/// The function allocates one page block from the allocator.
	///
	/// \return Allocated page block.
	page_frame_t *get();


	/// \brief Release page block.
	///
	/// The function release page block and puts to the allocator with
	/// page block squashing.
	///
	/// \param frame Pointer to the page block to be released.
	/// \returns Pointer to squashed page block is squashing is possible;
	/// nullptr otherwise.
	page_frame_t *put(page_frame_t *frame);


private:

	/// Type of list element.
	struct item_type;


	/// Find last lesser.
	void find_last_lesser(const item_type *new_item,
			item_type * &last_lesser,
			item_type * &prelast_lesser) const;


	/// Run sanity check.
	void sanity_check() const;


	/// Pointer to the first item in the list.
	item_type *list_ = nullptr;


	/// Order of the list.
	size_t order_ = 0;

	bool disable_squashing_ = false;


	/// Output operator for \ref FreeLists
	friend
	cio::OutStream& memory::allocators::operator<<(cio::OutStream& stream,
				const FreeList& fl);
}; // class FreeList

cio::OutStream& operator<<(cio::OutStream& stream,
		const FreeList& fl);


} // namespace allocators


} // namespace memory
