#pragma once

#include <gtest/gtest.h>

#include <lib/type_traits.hpp>

namespace testing {


namespace ____impl {

template<class N>
struct has_member_next
{
private:
	template<typename T>
	static lib::true_type test(decltype(T::next)*) {}

	template<typename>
	static lib::false_type test(...) {}

public:
	static constexpr bool value = decltype(test<N>(nullptr))::value;
};

template<class N>
constexpr bool has_member_next<N>::value;


};


namespace _impl {


struct basic_forward_list_node
{
	basic_forward_list_node* next = nullptr;
};


} // namesapce _impl

template<class Node>
struct basic_forward_list {
	// TODO: add compile-time checking of inheritance Node
	// from basic_forward_list_node

	using node_type = Node;


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


	// removes node and returns removed node
	inline
	node_type* pop_front()
	{
		auto removed = before_begin_->next;
		before_begin_->next = removed->next;
		return static_cast<node_type*>(removed);
	}


	inline
	node_type* front()
	{
		return static_cast<node_type*>(before_begin_->next);
	}

	node_type* before_begin_;
}; // class basic_forward_list


} // namespace testing
