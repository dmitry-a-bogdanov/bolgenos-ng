#pragma once


#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/cout.hpp>


namespace lib {

template<typename ValueType>
class list {
	struct list_item_type; // forward declaration
public:
	using value_type = ValueType;
	class iterator {
	public:
		iterator() = delete;
		explicit iterator(list_item_type *position)
			: position_(position) {
		}

		iterator(const iterator &orig)
			: position_(orig.position_) {
		}

		iterator& operator =(const iterator &orig) {
			position_ = orig.position_;
			return *this;
		}

		iterator& operator ++() {
			position_ = position_->next_;
			return *this;
		}

		value_type& operator *() {
			return position_->value_;
		}


		value_type* operator ->() {
			return &position_->value_;
		}

		bool operator !=(const iterator& other) {
			return position_ != other.position_;
		}


		bool operator ==(const iterator& other) {
			return position_ == other.position_;
		}
	private:
		list_item_type *position_;
	};

	list() = default;

	list(const list &) = delete;
	list& operator =(const list &) = delete;

	bool push_front(const value_type &);
	bool push_front(value_type &&);

	iterator begin() {
		return iterator(first_);
	}

	iterator end() {
		return iterator(nullptr);
	}
private:
	struct list_item_type {
		list_item_type *next_ = nullptr;
		value_type value_;
	};

	list_item_type *first_ = nullptr;
};


template<typename ValueType>
bool list<ValueType>::push_front(const value_type &value) {
	list_item_type *new_elem = static_cast<list_item_type *>(
			memory::kmalloc(sizeof(list_item_type)));
	if (!new_elem) {
		cio::cerr << __func__ << ": " << "allocation failed" << cio::endl;
		return false;
	}
	new_elem->next_ = first_;
	new_elem->value_ = value;
	first_ = new_elem;
	return true;
}

} // namespace lib
