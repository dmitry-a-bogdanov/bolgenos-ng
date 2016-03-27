#pragma once


#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/cout.hpp>


namespace lib {


/// Linked (one-directional) list.
///
/// \tparam ValueType type of value to hold in list.
template<typename ValueType>
class list {
	struct list_item_type; // forward declaration
public:
	/// Alias for ValueType.
	using value_type = ValueType;


	/// Iterator class.
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


	/// Constructor without parameters.
	list() = default;


	/// Copy-constructing is denied.
	list(const list &) = delete;


	/// Copy-assignment is denied.
	list& operator =(const list &) = delete;


	/// Destructor that frees memory allocated for list.
	~list();


	/// \brief Push to the front.
	///
	/// Push new element in front of list. Function may fail if allocation
	/// of new element is failed.
	///
	/// \param value value to be pushed in the list
	/// \return true if element is added successfully;
	/// false otherwise.
	bool push_front(const value_type &value);


	/// Get iterator that points to the begging of the container.
	iterator begin() {
		return iterator(first_);
	}


	/// Get iterator that points to the end of the container.
	iterator end() {
		return iterator(nullptr);
	}


	bool empty() {
		return first_ == nullptr;
	}
private:
	struct list_item_type {
		list_item_type *next_ = nullptr;
		value_type value_;
	};

	list_item_type *first_ = nullptr;
};


template<typename ValueType>
list<ValueType>::~list() {
	for(auto it = first_; it != nullptr;) {
		auto next = it->next_;
		memory::kfree(it);
		it = next;
	}
}


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
