#pragma once

#include "memory.hpp"
#include "impl/basic_forward_list.hpp"

namespace lib {


namespace _impl {


template<class T>
struct fwd_list_node:
	public basic_fwd_list_node
{
	T value_{};
	virtual ~fwd_list_node() {}
};



} // namespace _impl



template<class T, class Alloc = default_allocator<T>>
class forward_list:
	private lib::_impl::basic_forward_list<
		lib::_impl::fwd_list_node<T>>
{
	using node_type = lib::_impl::fwd_list_node<T>;
	using base_list = lib::_impl::basic_forward_list<node_type>;
public:
	using value_type = T;
	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;
	using error_type = bool;

	class iterator;
	class const_iterator;

	forward_list() = default;


	forward_list(const forward_list&) = delete;
	forward_list(forward_list&&) = delete;
	forward_list& operator=(const forward_list&) = delete;

	~forward_list()
	{
		while (!empty()) {
			pop_front();
		}
	}


	inline
	reference front()
	{
		return base_list::begin()->value_;
	}


	inline
	const_reference front() const
	{
		return base_list::begin()->value_;
	}


	inline
	void erase_after(const_iterator position)
	{
		auto after = const_cast<node_type *>(position.current_);
		auto erased = base_list::erase_after(after);
		alloc_.deallocate(erased, 1);
	}


	inline
	void pop_front()
	{
		erase_after(before_begin());
	}

	inline
	iterator push_after(const_iterator position,
		const value_type& value)
	{
		node_type *node = alloc_.allocate(1);
		if (!node) {
			return end();
		}

		node->value_ = value;

		auto after = const_cast<node_type *>(position.current_);
		iterator pointer_to_inserted =
			base_list::push_after(after, node);
		return pointer_to_inserted;
	}


	inline
	iterator push_front(const value_type& value)
	{
		return push_after(before_begin(), value);
	}


	inline
	bool empty() const
	{
		return base_list::begin() == base_list::end();
	}


	inline
	iterator before_begin()
	{
		return iterator(base_list::before_begin());
	}


	inline
	const_iterator before_begin() const
	{
		return cbefore_begin();
	}


	inline
	const_iterator cbefore_begin() const
	{
		return const_iterator(base_list::before_begin());
	}


	inline
	iterator begin()
	{
		return iterator(base_list::begin());
	}


	inline
	const_iterator begin() const
	{
		return const_iterator(base_list::begin());
	}


	inline
	const_iterator cbegin() const
	{
		return begin();
	}


	inline
	iterator end()
	{
		return iterator(base_list::end());
	}


	inline
	const_iterator end() const
	{
		return const_iterator(base_list::end());
	}


	inline
	const_iterator cend() const
	{
		return const_iterator(base_list::end());
	}

	size_t remove(const T& value);

	void clear() {
		while (!empty()) {
			erase_after(before_begin());
		}
	}

private:

	typename allocator_type::template
		rebind<node_type>::other alloc_= {};

}; // class forward_list


template<class T, class Alloc>
class forward_list<T, Alloc>::const_iterator
{
public:
	const_iterator() = default;

	const_iterator(const iterator& other) :
		current_(other.current_)
	{
	}


	inline
	const_reference operator*()
	{
		return current_->value_;
	}


	inline
	const_iterator& operator++()
	{
		current_ = static_cast<decltype(current_)>(current_->next);
		return *this;
	}


	inline
	const_iterator operator++(int)
	{
		const_iterator tmp(*this);
		current_ = static_cast<decltype(current_)>(current_->next);
		return tmp;
	}


	inline
	bool operator==(const const_iterator& other) const
	{
		return current_ == other.current_;
	}


	inline
	bool operator==(const iterator& other) const
	{
		return current_ == other.current_;
	}


	inline
	bool operator!=(const const_iterator& other) const
	{
		return current_ != other.current_;
	}


	inline
	bool operator!=(const iterator& other) const
	{
		return current_ != other.current_;
	}

private:
	const_iterator(const node_type* node) :
		current_(node)
	{
	}


	const node_type* current_ = base_list::end();


	friend
	class iterator;


	friend
	class forward_list;
};


template<class T, class Alloc>
class forward_list<T, Alloc>::iterator
{
public:
	iterator() = default;

	inline
	reference operator*()
	{
		return current_->value_;
	}


	inline
	iterator& operator++()
	{
		current_ = static_cast<decltype(current_)>(current_->next);
		return *this;
	}


	inline
	iterator operator++(int)
	{
		iterator tmp(*this);
		current_ = static_cast<decltype(current_)>(current_->next);
		return tmp;
	}


	inline
	bool operator==(const iterator& other) const
	{
		return current_ == other.current_;
	}


	inline
	bool operator==(const const_iterator& other) const
	{
		return current_ == other.current_;
	}


	inline
	bool operator!=(const iterator& other) const
	{
		return current_ != other.current_;
	}


	inline
	bool operator!=(const const_iterator& other) const
	{
		return current_ != other.current_;
	}

private:
	iterator(node_type* node) :
		current_(node)
	{
	}


	node_type* current_ = nullptr;


	friend
	class const_iterator;


	friend
	class forward_list;
};

template<typename InputIt>
constexpr void advance(InputIt& it, int32_t n = 1) {
	while (n > 0) {
		++it;
		--n;
	}
}

template<typename InputIt>
constexpr InputIt next(InputIt it, int32_t n = 1) {
	advance(it, n);
	return it;
}

template<class T, class Alloc>
size_t forward_list<T, Alloc>::remove(const T& value){
	size_t removed = 0;
	auto it = before_begin();
	while (next(it) != end()) {
		auto next_it = next(it);
		if (next_it == end()) {
			break;
		} else if (*next_it == value) {
			erase_after(it);
			++removed;
		} else {
			++it;
		}
	}
	return removed;
}

} // namespace lib
