#include "free_list.hpp"

struct memory::allocators::FreeList::item_type {
	/// Pointer to the next element.
	item_type *next;


	/// Clear internal allocator's data in item
	/// for security reasons.
	void clear() {
		next = nullptr;
	}
};

namespace {


/// Check that frames are consequent.
constexpr bool are_consequent(const memory::page_frame_t *first,
		const memory::page_frame_t *second, size_t order);


/// The function is the first in the pair of squashed pages.
constexpr bool is_the_first_in_buddy(const memory::page_frame_t *frame,
		size_t order);

} // namespace


bool memory::allocators::FreeList::initialize(size_t order,
		bool disable_squashing) {
	list_ = nullptr;
	order_ = order;
	disable_squashing_ = disable_squashing;
	sanity_check();
	return true;
}

memory::page_frame_t *memory::allocators::FreeList::get() {
	item_type *free_item = list_;
	if (free_item) {
		list_ = list_->next;
		free_item->next = nullptr;
		--stats.items;
	}
	sanity_check();
	return reinterpret_cast<page_frame_t *>(free_item);
}


memory::page_frame_t *memory::allocators::FreeList::put(page_frame_t *frame) {
	auto new_item = reinterpret_cast<item_type *>(frame);
	new_item->next = nullptr;

	if (list_ == nullptr) {
		list_ = new_item;
		++stats.items;
		sanity_check();
		return nullptr;
	}

	item_type *last_lesser_item;
	item_type *prelast_lesser_item;
	find_last_lesser(new_item, last_lesser_item, prelast_lesser_item);

	if (disable_squashing_) {
		if (last_lesser_item == nullptr) {
			new_item->next = list_;
			list_ = new_item;
		} else {
			new_item->next = last_lesser_item->next;
			last_lesser_item->next = new_item;
		}
		++stats.items;
		sanity_check();
		return nullptr;
	}

	auto last_lesser_frame =
			reinterpret_cast<page_frame_t *>(last_lesser_item);

	if (last_lesser_item == nullptr) {
		// Element should be placed to the beginning of the list.
		if (are_consequent(frame,
				reinterpret_cast<page_frame_t *>(list_),
				order_)
				&& is_the_first_in_buddy(frame, order_)) {
			list_ = list_->next;
			--stats.items;
			sanity_check();
			return frame;
		} else {
			new_item->next = list_;
			list_ = new_item;
			++stats.items;
			sanity_check();
			return nullptr;
		}
	}

	auto next_item = last_lesser_item->next;
	auto next_frame = reinterpret_cast<page_frame_t *>(next_item);

	if (are_consequent(last_lesser_frame, frame, order_)
			&& is_the_first_in_buddy(last_lesser_frame, order_)) {
		if (prelast_lesser_item) {
			prelast_lesser_item->next = last_lesser_item->next;
			last_lesser_item->next = nullptr;
			--stats.items;
			sanity_check();
			return last_lesser_frame;
		} else {
			list_ = last_lesser_item->next;
			last_lesser_item->next = nullptr;
			--stats.items;
			sanity_check();
			return last_lesser_frame;
		}
	}

	if (are_consequent(frame, next_frame, order_)
			&& is_the_first_in_buddy(frame, order_)) {
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


void memory::allocators::FreeList::find_last_lesser(const item_type *new_item,
		item_type * &last_lesser, item_type * &prelast_lesser) const {
	prelast_lesser = nullptr;
	last_lesser = nullptr;
	for (auto item = list_; item && item < new_item; item = item->next) {
		prelast_lesser = last_lesser;
		last_lesser = item;
	}
}


void memory::allocators::FreeList::sanity_check() const {
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
	} while (slow_iter && fast_iter && slow_iter != fast_iter);

	if (slow_iter == fast_iter && slow_iter) {
		panic("FreeList loop detected!");
	}


	size_t length = 0;
	for (item_type *it = list_; it; it = it->next)
		length++;
	if (length != stats.items) {
		cio::ccrit << "actual = " << length
			<< " expected = " << stats.items << cio::endl;
		panic("FreeList stats are invalid!");
	}
}



cio::OutStream& memory::allocators::operator<<(cio::OutStream& stream,
			const FreeList& fl) {
	stream << "[FreeList(" << fl.order_ << ", " << fl.disable_squashing_
		<< "):";
	for (auto *item = fl.list_; item; item = item->next) {
		stream << " " << item;
	}
	stream << "]";
	return stream;
}


namespace {


constexpr bool are_consequent(const memory::page_frame_t *first,
		const memory::page_frame_t *second, size_t order) {
	return (second - first) == (1 << order);
}


constexpr bool is_the_first_in_buddy(const memory::page_frame_t *frame,
		size_t order) {
	return ! ((frame - reinterpret_cast<memory::page_frame_t *>(0))
			& (1 << order));
}

} // namespace
