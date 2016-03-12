#pragma once

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/stdtypes.hpp>

#include "config.h"


namespace memory {


namespace allocators {


/// \brief Free list allocator.
///
/// The structure provides functionality of free list list allocator. Such
/// allocator keeps one-directional list of blocks of page frames.
/// \tparam MaxOrder Maximal order of the free list in set. TODO: to remove.
template<size_t MaxOrder>
class FreeList {
	struct item_type; // forward declaration.
public:
	struct max_order {
		/// Maximal order of the free list in set. TODO: to remove.
		static constexpr size_t value = MaxOrder;
	};


	/// Structure that holds statistics of the usage of the free list.
	struct stats_type {
		/// Number of elements in list.
		size_t items = 0;
	};


	/// Static of the usage of this free list.
	stats_type stats;


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
	/// \return boolean status of initalization. true if success; false
	/// otherwise.
	inline bool initialize(size_t order) {
		if (order > max_order::value) {
			return false;
		}
		list_ = nullptr;
		order_ = order;
		return true;
	}


	/// \brief Get next free page block.
	///
	/// The function allocates one page block from the allocator.
	///
	/// \return Allocated page block.
	inline page_frame_t *get() {
		item_type *free_item = list_;
		if (free_item) {
			list_ = list_->next;
			free_item->next = nullptr;
			//--stats.items;
		}
		sanity_check();
		return reinterpret_cast<page_frame_t *>(free_item);
	}


	/// \brief Release page block.
	///
	/// The function release page block and puts to the allocator with
	/// page block squashing.
	///
	/// \param frame Pointer to the page block to be released.
	/// \returns Pointer to squashed page block is squashing is possible;
	/// nullptr otherwise.
	page_frame_t *put(page_frame_t *frame) {
		auto new_item = reinterpret_cast<item_type *>(frame);
		new_item->next = nullptr;

		if (list_ == nullptr) {
			list_ = new_item;
			//++stats.items;
			sanity_check();
			return nullptr;
		}

		item_type *last_lesser_item;
		item_type *prelast_lesser_item;
		find_last_lesser(new_item, last_lesser_item,
			prelast_lesser_item);

		if (order_ == max_order::value) {
			// this FreeList is a free list of max order.
			// it means that allocators of higher orders don't
			// exist and we just puts frame to list.
			new_item->next = last_lesser_item->next;
			last_lesser_item->next = new_item;
			++stats.items;
			sanity_check();
			return nullptr;
		}


		auto last_lesser_frame
			= reinterpret_cast<page_frame_t *>(last_lesser_item);

		if (last_lesser_item == nullptr) {
			// Element should be placed to the beginning of the list.
			if (are_consequent(frame, reinterpret_cast<page_frame_t *>(list_))
				&& is_the_first_in_buddy(frame)) {
				list_ = list_->next;
				--stats.items;
				sanity_check();
				return frame;
			} else {
				new_item->next = list_;
				list_ = new_item;
				//++stats.items;
				sanity_check();
				return nullptr;
			}
		}

		auto next_item = last_lesser_item->next;
		auto next_frame = reinterpret_cast<page_frame_t *>(next_item);

		if (are_consequent(last_lesser_frame, frame)
				&& is_the_first_in_buddy(last_lesser_frame)) {
			--stats.items;
			if (prelast_lesser_item) {
				prelast_lesser_item->next = last_lesser_item->next;
				last_lesser_item->next = nullptr;
				sanity_check();
				return last_lesser_frame;
			} else {
				list_ = last_lesser_item->next;
				last_lesser_item->next = nullptr;
				sanity_check();
				return last_lesser_frame;
			}
		}

		if (are_consequent(frame, next_frame)
				&& is_the_first_in_buddy(frame)) {
			last_lesser_item->next = next_item->next;
			--stats.items;
			sanity_check();
			return frame;
		}


		new_item->next = last_lesser_item->next;
		last_lesser_item->next = new_item;
		++stats.items;
		sanity_check();

		return nullptr;
	}


private:

	/// Find last lesser.
	void find_last_lesser(const item_type *new_item, item_type * &last_lesser,
			item_type * &prelast_lesser) const {
		prelast_lesser = nullptr;
		last_lesser = nullptr;
		for(auto item = list_; item && item < new_item;
				item = item->next) {
			prelast_lesser = last_lesser;
			last_lesser = item;
		}
	}

	/// Run sanity check.
	void sanity_check() const {
		item_type *fast_iter = list_, *slow_iter = list_;
		do {
			if (slow_iter) {
				slow_iter = slow_iter->next;
			}
			if (fast_iter) {
				fast_iter = fast_iter->next;
			}
			if (fast_iter) {
				fast_iter = fast_iter->next;
			}
		} while(slow_iter && fast_iter && slow_iter != fast_iter);

		if (slow_iter == fast_iter && slow_iter) {
			panic("FreeList loop detected!");
		}
	}


	/// Check that frames are consequent.
	bool are_consequent(const page_frame_t *first,
			const page_frame_t *second)
	{
		const auto diff_frames = 1 << order_;
		return second - first == diff_frames;
	}


	/// The function is the first in the pair of squashed pages.
	bool is_the_first_in_buddy(const page_frame_t *frame) const {
		auto numeric = reinterpret_cast<size_t>(frame);
		auto divided = numeric / (PAGE_SIZE*(1 << order_));
		return divided % 2 == 0;
	}


	/// Type of list element.
	struct item_type {
		/// Pointer to the next element.
		item_type *next;


		/// Clear internal allocator's data in item
		/// for security reasons.
		void clear() {
			next = nullptr;
		}
	};


	/// Pointer to the first item in the list.
	item_type *list_ = nullptr;


	/// Order of the list.
	size_t order_ = 0;


	/// Output operator for \ref FreeLists
	friend
	cio::OutStream& operator<<(cio::OutStream& stream,
				const FreeList<max_order::value>& fl) {
		stream << "[FreeList<" << max_order::value << ">"
			<< "(" << fl.order_ << "):";
		for (auto *item = fl.list_; item; item = item->next) {
			stream << " " << item;
		}
		stream << "]";
		return stream;
	}
}; // class FreeList


} // namespace allocators


} // namespace memory
