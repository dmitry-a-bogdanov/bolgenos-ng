#include <forward_list.hpp>

#include <gtest/gtest.h>



TEST(read_access, first_element)
{
	const int test_value = 10;
	bolgenos_testing::forward_list<int> l;
	l.push_front(test_value);
	EXPECT_EQ(test_value, *l.begin());
	const bolgenos_testing::forward_list<int>& cref_l = l;
	EXPECT_EQ(test_value, *cref_l.begin());

	EXPECT_EQ(l.cbegin(), cref_l.cbegin());
	EXPECT_EQ(l.cbegin(), cref_l.begin());
}


TEST(read_access, sequental_access)
{
	using list_type = bolgenos_testing::forward_list<int>;
	list_type l;
	const list_type& cref_l = l;

	for(int i = 0; i < 10; ++i) {
		l.push_front(i);
	}

	list_type::const_iterator	l_cbb =		l.cbefore_begin();
	//list_type::iterator		l_bb =		l.before_begin();
	list_type::const_iterator	cref_l_cbb =	cref_l.cbefore_begin();
	list_type::const_iterator	cref_l_bb =	cref_l.before_begin();

	list_type::const_iterator	l_cb =		l.cbegin();
	//list_type::iterator		l_b =		l.begin();
	list_type::const_iterator	cref_l_cb =	cref_l.cbegin();
	list_type::const_iterator	cref_l_b =	cref_l.begin();

	++l_cbb;
	// ++l_bb;
	++cref_l_cbb;
	++cref_l_bb;

	for(int i = 9; i >= 0; --i)
	{
		EXPECT_EQ(i, *l_cbb);
		//EXPECT_EQ(i, *l_bb);
		EXPECT_EQ(i, *l_cb);
		//EXPECT_EQ(i, *l_b);

		EXPECT_EQ(i, *cref_l_cbb);
		EXPECT_EQ(i, *cref_l_bb);
		EXPECT_EQ(i, *cref_l_cb);
		EXPECT_EQ(i, *cref_l_b);

		++l_cb;
		//++l_b;
		++cref_l_cb; ++cref_l_b;
		++l_cbb;
		//++l_bb;
		++cref_l_cbb;
		++cref_l_bb;
	}


	EXPECT_EQ(l.cend(), l_cbb);
	//EXPECT_EQ(l.end(), l_bb);
	EXPECT_EQ(l.cend(), cref_l_cbb);
	EXPECT_EQ(l.cend(), cref_l_bb);
	EXPECT_EQ(l.cend(), l_cb);
	//EXPECT_EQ(l.end(), l_b);
	EXPECT_EQ(l.cend(), cref_l_cb);
	EXPECT_EQ(l.cend(), cref_l_b);
}
