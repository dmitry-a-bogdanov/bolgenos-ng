#pragma once

#include <cstddef>

#include "memory.hpp"

namespace testing {


template<class T, class Alloc = default_allocator<T>>
class forward_list
{
public:
	using value_type = T;
	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;
	using error_type = bool;

	forward_list() = default;
	forward_list(const forward_list&) = delete;
	forward_list& operator=(const forward_list&) = delete;
	~forward_list();

	inline reference front()
	{
		return first_->value_;
	}

	inline const_reference front() const
	{
		return first_->value_;
	}

	void pop_front();
	error_type push_front(const value_type&);

	inline bool empty() const
	{
		return first_ == nullptr;
	}

private:
	struct list_item_type;
	list_item_type* first_ = nullptr;
	typename allocator_type::template
		rebind<list_item_type>::other alloc_= {};
}; // class forward_list


template<class T, class Alloc>
struct forward_list<T, Alloc>::list_item_type
{
	T value_;
	list_item_type* next_;
};


template<class T, class Alloc>
forward_list<T, Alloc>::~forward_list()
{
	for (auto it = first_; it != nullptr;) {
		auto next = it->next_;
		alloc_.deallocate(it, 1);
		it = next;
	}
}


template<class T, class Alloc>
void forward_list<T, Alloc>::pop_front()
{
	auto removed_elem = first_;
	first_ = first_->next_;
	alloc_.deallocate(first_, 1);
}


template<class T, class Alloc>
bool forward_list<T, Alloc>::push_front(const value_type& value)
{
	list_item_type *elem = alloc_.allocate(1);
	if (!elem) {
		return false;
	}
	elem->next_ = first_;
	elem->value_ = value;
	first_ = elem;
	return true;
}


} // namespace testing
