#pragma once

#include "memory.hpp"
#include "impl/basic_forward_list.hpp"

namespace testing {


namespace _impl {


template<class T>
struct fwd_list_node:
	public basic_fwd_list_node
{
	T value_;
	virtual ~fwd_list_node() {}
};


} // namespace _impl


template<class T, class Alloc = default_allocator<T>>
class forward_list:
	private testing::_impl::basic_forward_list<
		testing::_impl::fwd_list_node<T>>
{
	using node_type = testing::_impl::fwd_list_node<T>;
	using base_list = testing::_impl::basic_forward_list<node_type>;
	//template<class,class>
	//class
	//_fwd_list_iterator;
public:
	using value_type = T;
	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;
	using error_type = bool;
	/*
	using iterator = _fwd_list_iterator<value_type,
		reference>;
	using const_iterator = _fwd_list_iterator<const value_type,
		const_reference>;
	*/

	forward_list():
		base_list(&before_begin_)
	{
	}


	forward_list(const forward_list&) = delete;
	forward_list& operator=(const forward_list&) = delete;
	~forward_list();


	inline
	reference front()
	{
		return base_list::front()->value_;
	}


	inline
	const_reference front() const
	{
		return base_list::front()->value_;
	}


	void pop_front();
	error_type push_front(const value_type&);


	inline
	bool empty() const
	{
		return base_list::front() == nullptr;
	}

#if 0

	inline
	iterator before_begin()
	{
		return iterator(base_list::front());
	}


	inline
	const_iterator before_begin() const
	{
		return cbefore_begin();
	}


	inline
	const_iterator cbefore_begin() const
	{
		return const_iterator(&base_list::before_begin());
	}

	inline
	iterator begin()
	{
		return iterator(base_list::first());
	}


	inline
	const_iterator begin() const
	{
		return const_iterator(base_list::first);
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

#endif

private:

	node_type before_begin_ = {};
	typename allocator_type::template
		rebind<node_type>::other alloc_= {};


/*
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
*/
}; // class forward_list


template<class T, class Alloc>
forward_list<T, Alloc>::~forward_list()
{
	while (!empty()) {
		auto removed = base_list::pop_front();
		alloc_.deallocate(removed, 1);
	}
}


template<class T, class Alloc>
void forward_list<T, Alloc>::pop_front()
{
	auto removed = base_list::pop_front();
	alloc_.deallocate(removed, 1);
}


template<class T, class Alloc>
bool forward_list<T, Alloc>::push_front(const value_type& value)
{
	node_type *node = alloc_.allocate(1);
	node->value_ = value;
	if (!node) {
		return false;
	}
	base_list::push_front(node);
	return true;
}


} // namespace testing
