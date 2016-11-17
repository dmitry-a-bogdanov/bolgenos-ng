#include "basic_forward_list.hpp"


template<typename T>
struct simple_node:
	public testing::_impl::basic_forward_list_node
{
	T value_;
};



template<class T>
class simple_list:
	private testing::basic_forward_list<simple_node<T>>
{
	using node_type = simple_node<T>;
	using base_list = testing::basic_forward_list<node_type>;
public:
	using value_type = T;

	simple_list()
		: base_list(&before_begin_)
	{
	}


	void push_front(const value_type& val)
	{
		auto node = new node_type;
		node->value_ = val;
		base_list::push_front(node);
	}


	void pop_front()
	{
		auto removed_node = base_list::pop_front();
		delete removed_node;
	}


	T front()
	{
		return base_list::front()->value_;
	}

	node_type before_begin_;
};


TEST(basic_forward_list, push_and_pop_10)
{
	const int test_size = 10;

	simple_list<int> l;

	for (int i = 0; i < test_size; ++i)
	{
		l.push_front(i);
		EXPECT_EQ(i, l.front());
	}

	for (int i = test_size - 1; i >= 0; --i)
	{
		EXPECT_EQ(i, l.front());
		l.pop_front();
	}
}


class simple_free_list:
	public testing::basic_forward_list<
		testing::_impl::basic_forward_list_node>
{
	using node_type = testing::_impl::basic_forward_list_node;
	using base_list = testing::basic_forward_list<node_type>;
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


TEST(simple_free_list, push_and_pop_10)
{
	const int test_size = 10;

	simple_free_list l;

	char* allocated[test_size];

	for (int i = 0; i < test_size; ++i)
	{
		allocated[i] = new char[64];
		l.push_front(allocated[i]);
		EXPECT_EQ(allocated[i], l.front()) << "i = " << i;
	}

	for (int i = test_size - 1; i >= 0; --i)
	{
		EXPECT_EQ(allocated[i], l.front());
		l.pop_front();
		delete[] allocated[i];
	}
}
