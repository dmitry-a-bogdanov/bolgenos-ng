#pragma once

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/stdtypes.hpp>

#include "config.h"

namespace memory {
namespace allocators {


template<size_t MaxOrder>
class FreeList {
	struct item_type;
public:
	struct max_order {
		static constexpr size_t value = MaxOrder;
	};
/*
	struct stats_type {
		size_t items = 0;
	};
	stats_type stats;
*/

	FreeList() = default;
	FreeList(const FreeList &) = delete;
	FreeList& operator =(const FreeList &) = delete;
	~FreeList() {}


	inline bool initialize(size_t order) {
		if (order > max_order::value) {
			return false;
		}
		list_ = nullptr;
		order_ = order;
		return true;
	}


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
			//++stats.items;
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
				//--stats.items;
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
			//--stats.items;
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
			//--stats.items;
			sanity_check();
			return frame;
		}


		new_item->next = last_lesser_item->next;
		last_lesser_item->next = new_item;
//		++stats.items;
		sanity_check();

		return nullptr;
	}

private:
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


	bool are_consequent(const page_frame_t *first,
			const page_frame_t *second)
	{
		const auto diff_frames = 1 << order_;
		return second - first == diff_frames;
	}

	bool is_the_first_in_buddy(const page_frame_t *frame) const {
		auto numeric = reinterpret_cast<size_t>(frame);
		auto divided = numeric / (PAGE_SIZE*(1 << order_));
		return divided % 2 == 0;
	}

	struct item_type {
		item_type *next;
		void clear() {
//			next = nullptr;
		}
	};

	item_type *list_ = nullptr;
	size_t order_ = 0;

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
