#pragma once

#include <cstddef>

#include "memory.hpp"

namespace testing {


template<class T, class Alloc = default_allocator<T>>
class forward_list
{
	template<class,class>
	class
	_fwd_list_iterator;
public:
	using value_type = T;
	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;
	using error_type = bool;
	using iterator = _fwd_list_iterator<value_type,
		reference>;
	using const_iterator = _fwd_list_iterator<const value_type,
		const_reference>;

	forward_list() = default;
	forward_list(const forward_list&) = delete;
	forward_list& operator=(const forward_list&) = delete;
	~forward_list();


	inline
	reference front()
	{
		return first_->value_;
	}


	inline
	const_reference front() const
	{
		return first_->value_;
	}


	void pop_front();
	error_type push_front(const value_type&);


	inline
	bool empty() const
	{
		return first_ == nullptr;
	}


	inline
	iterator before_begin()
	{
		return iterator(&before_begin_);
	}


	inline
	const_iterator before_begin() const
	{
		return cbefore_begin();
	}


	inline
	const_iterator cbefore_begin() const
	{
		return const_iterator(&before_begin_);
	}

	inline
	iterator begin()
	{
		return iterator(first_);
	}


	inline
	const_iterator begin() const
	{
		return const_iterator(first_);
	}


	inline
	const_iterator cbegin() const
	{
		return begin();
	}


	inline
	iterator end()
	{
		return iterator(nullptr);
	}


	inline
	const_iterator end() const
	{
		return const_iterator(nullptr);
	}


	inline
	const_iterator cend() const
	{
		return const_iterator(nullptr);
	}


private:

	struct list_item_type;
	list_item_type before_begin_;
	list_item_type*& first_ = before_begin_.next_;
	typename allocator_type::template
		rebind<list_item_type>::other alloc_= {};



	template<class ValueType, class Reference = ValueType&>
	class _fwd_list_iterator
	{
	public:
		using value_type = ValueType;
		using reference = Reference;

		_fwd_list_iterator() = default;

		_fwd_list_iterator(const _fwd_list_iterator& other)
			: current_(other.current_)
		{
		}


		inline
		_fwd_list_iterator& operator=(const _fwd_list_iterator& other)
		{
			current_ = other.current_;
			return *this;
		}


		~_fwd_list_iterator()
		{
			current_ = nullptr;
		}


		inline
		_fwd_list_iterator& operator++()
		{
			current_ = current_->next_;
		}


		inline
		_fwd_list_iterator& operator++(int)
		{
			iterator tmp(*this);
			++(*this);
			return tmp;
		}


		inline
		reference operator*()
		{
			return const_cast<reference>(current_->value_);
		}


		inline
		bool operator==(const _fwd_list_iterator& other) const
		{
			return current_ == other.current_;
		}


	private:
		explicit _fwd_list_iterator(const list_item_type* node)
		{
			current_ = node;
		}

		const list_item_type *current_ = nullptr;

		friend class testing::forward_list<T, Alloc>;
	};

}; // class forward_list


template<class T, class Alloc>
struct forward_list<T, Alloc>::list_item_type
{
	T value_;
	list_item_type* next_ = nullptr;
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
