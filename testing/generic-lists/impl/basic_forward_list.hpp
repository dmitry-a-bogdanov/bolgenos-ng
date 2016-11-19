#pragma once

#include <gtest/gtest.h>

#include <lib/type_traits.hpp>

namespace bolgenos_testing {




namespace _impl {


struct basic_fwd_list_node
{
	basic_fwd_list_node* next = nullptr;

	// just to make it virtual
	virtual ~basic_fwd_list_node() {}
};


template<class Node>
struct basic_forward_list {
	// TODO: add compile-time checking of inheritance Node
	// from basic_fwd_list_node

	using node_type = Node;
	using pointer = node_type*;
	using const_pointer = const node_type*;


	basic_forward_list() = delete;
	basic_forward_list(const basic_forward_list&)  = delete;
	basic_forward_list(node_type* before_begin)
		: before_begin_(before_begin)
	{
	}

	// adds node
	inline
	void push_front(node_type *new_node)
	{
		new_node->next = before_begin_->next;
		before_begin_->next = new_node;
	}


	inline
	pointer push_after(node_type* after, node_type* node)
	{
		node->next = after->next;
		after->next = node->next;
	}


	// removes node and returns removed node
	inline
	node_type* pop_front()
	{
		auto removed = before_begin_->next;
		before_begin_->next = removed->next;
		return static_cast<node_type*>(removed);
	}


	inline
	pointer front()
	{
		return static_cast<pointer>(before_begin_->next);
	}


	inline
	const_pointer front() const
	{
		return static_cast<const_pointer>(before_begin_->next);
	}


	inline
	pointer first()
	{
		return static_cast<pointer>(before_begin_->next);
	}


	inline
	const_pointer first() const
	{
		return static_cast<const_pointer>(before_begin_->next);
	}


	inline
	pointer before_begin()
	{
		return before_begin_;
	}


	inline
	pointer end()
	{
		return nullptr;
	}


	inline
	const_pointer end() const
	{
		return nullptr;
	}


	inline
	const_pointer before_begin() const
	{
		return before_begin_;
	}


	node_type* before_begin_;
}; // class basic_forward_list


} // namesapce _impl


} // namespace bolgenos_testing
