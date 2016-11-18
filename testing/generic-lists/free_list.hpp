#pragma once

#include "impl/basic_forward_list.hpp"



namespace testing {


class simple_free_list:
	public testing::_impl::basic_forward_list<
		testing::_impl::basic_fwd_list_node>
{
	using node_type = testing::_impl::basic_fwd_list_node;
	using base_list = testing::_impl::basic_forward_list<node_type>;
public:
	simple_free_list()
		: base_list(&before_begin_)
	{
	}


	void push_front(void *page)
	{
		auto new_node = static_cast<node_type *>(page);
		base_list::push_front(new_node);
	}


	void pop_front()
	{
		auto removed = base_list::pop_front();
		// FIXME: the following assignment will most probably be
		// ignored by compiler. Secure erase is required.
		removed->next = nullptr;
	}


	void *front()
	{
		return static_cast<void *>(base_list::front());
	}

	node_type before_begin_;
};




} // namespace testing
