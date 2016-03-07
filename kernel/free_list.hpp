#pragma once

#include <bolgenos-ng/stdtypes.hpp>
#include <bolgenos-ng/cout.hpp>


namespace memory {
namespace allocators {


template<size_t MaxOrder>
class FreeList {
	struct item_type;
public:
	// compile-time constant
	struct max_order {
		enum { value = MaxOrder };
	};

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
		auto free_item = reinterpret_cast<page_frame_t *>(list_);
		if (list_) {
			list_ = list_->next;
		}
		return free_item;
	}


	page_frame_t *put(page_frame_t *frame) {
		auto new_item = reinterpret_cast<item_type *>(frame);

		if (list_ == nullptr) {
			new_item->next = nullptr;
			list_ = new_item;
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
			return nullptr;
		}


		auto last_lesser_frame
			= reinterpret_cast<page_frame_t *>(last_lesser_item);
		auto next_item = last_lesser_item->next;
		auto next_frame = reinterpret_cast<page_frame_t *>(next_item);

		if (are_consequent(frame, next_frame)
				&& is_the_first_in_buddy(frame)) {
			last_lesser_item->next = next_item->next;
			new_item->clear();
			next_item->clear();
			return frame;
		}

		if (are_consequent(last_lesser_frame, frame)
				&& is_the_first_in_buddy(last_lesser_frame)) {
			if (prelast_lesser_item) {
				prelast_lesser_item->next
					= last_lesser_item->next;
				last_lesser_item->clear();
				new_item->clear();
				return last_lesser_frame;
			} else {
				list_ = last_lesser_item->next;
				last_lesser_item->clear();
				new_item->clear();
				return last_lesser_frame;
			}
		}


		if (last_lesser_frame) {
			new_item->next = last_lesser_item->next;
			last_lesser_item->next = new_item;
		} else {
			new_item->next = list_;
			list_ = new_item;
		}
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

	bool are_consequent(const page_frame_t *first,
			const page_frame_t *second)
	{
		const ptrdiff_t diff_frames = 1 << order_;
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
			next = nullptr;
		}
	};

	item_type *list_;
	size_t order_;

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
