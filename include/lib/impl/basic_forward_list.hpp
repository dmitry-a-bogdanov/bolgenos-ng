#pragma once

#include <lib/type_traits.hpp>

namespace lib {


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


	basic_forward_list() = default;
	basic_forward_list(const basic_forward_list&)  = delete;
	basic_forward_list(basic_forward_list&&) = delete;


	inline
	pointer push_after(node_type* after, node_type* node)
	{
		node->next = after->next;
		after->next = node;
		return node;
	}


	inline
	pointer erase_after(node_type* after)
	{
		auto erased = after->next;
		after->next = erased->next;
		return static_cast<pointer>(erased);
	}


	inline
	pointer begin()
	{
		return static_cast<pointer>(before_begin()->next);
	}


	inline
	const_pointer begin() const
	{
		return static_cast<const_pointer>(before_begin()->next);
	}


	inline
	pointer before_begin()
	{
		return &before_begin_;
	}


	inline
	const_pointer before_begin() const
	{
		return &before_begin_;
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


private:
	node_type before_begin_{};
}; // class basic_forward_list


} // namesapce _impl


} // namespace bolgenos_testing
