#pragma once

#include "impl/basic_forward_list.hpp"



namespace bolgenos_testing {


class free_list:
	public bolgenos_testing::_impl::basic_forward_list<
		bolgenos_testing::_impl::basic_fwd_list_node>
{
	using node_type = bolgenos_testing::_impl::basic_fwd_list_node;
	using base_list = bolgenos_testing::_impl::basic_forward_list<node_type>;
public:
	free_list()
		: base_list(&before_begin_)
	{
	}


	void push_front(void *page)
	{
		auto new_node = static_cast<node_type *>(page);
		base_list::push_after(&before_begin_, new_node);
	}


	void pop_front()
	{
		auto removed = base_list::pop_front();
		// FIXME: the following assignment will most probably be
		// ignored by compiler. Secure erase is required.
		removed->next = nullptr;
	}


	void* front()
	{
		return static_cast<void *>(base_list::front());
	}

private:

	node_type before_begin_;
};




} // namespace bolgenos_testing
