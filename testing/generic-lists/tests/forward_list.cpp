#include <forward_list.hpp>

#include <tuple>

#include <gtest/gtest.h>


TEST(basic, add_and_remove_10)
{
	bolgenos_testing::forward_list<int> l;
	const auto& cref = l;

	EXPECT_TRUE(l.empty());

	for (int i = 0; i < 10; ++i) {
		l.push_front(i);
		EXPECT_EQ(i, l.front());
		EXPECT_EQ(i, cref.front());
		EXPECT_FALSE(l.empty());
		EXPECT_FALSE(l.empty());
	}

	for(int i = 9; i >= 0; --i) {
		EXPECT_FALSE(l.empty());
		l.pop_front();
	}

	EXPECT_TRUE(l.empty());
}


//        end cend
// l
// cref_l


#include <iostream>

#if !defined(declare_true_eq)


#define test_debug 0

#if test_debug

#define		_declare_true_eq(x, y, m_x, m_y)			\
	std::clog << "EXPECT_TRUE("#x"."#m_x"() "			\
		"== "#y"."#m_y"());" << std::endl;			\
	EXPECT_TRUE(x.m_x() == y.m_y());				\
	std::clog << "EXPECT_FALSE("#x"."#m_x"() "			\
		"== "#y"."#m_y"());" << std::endl;			\
	EXPECT_FALSE(x.m_x() != y.m_y())

#else

#define		_declare_true_eq(x, y, m_x, m_y)			\
	EXPECT_TRUE(x.m_x() == y.m_y());				\
	EXPECT_FALSE(x.m_x() != y.m_y())

#endif

#define		_declare_true_eq1v1f(v, m)				\
	_declare_true_eq(v, v, m, m)

#define		_declare_true_eq1v2f(v, m1, m2)				\
	_declare_true_eq(v, v, m1, m2);					\
	_declare_true_eq(v, v, m2, m1)

#define		_declare_true_eq2v1f(v1, v2, m)				\
	_declare_true_eq(v1, v2, m, m);					\
	_declare_true_eq(v2, v1, m, m)

#define		_declare_true_eq2v2f(v1, v2, m1, m2)			\
	_declare_true_eq(v1, v2, m1, m2);				\
	_declare_true_eq(v1, v2, m2, m1);				\
	_declare_true_eq(v2, v1, m1, m2);				\
	_declare_true_eq(v2, v1, m2, m1)

#	define declare_true_eq(x, y, f1, f2)				\
	_declare_true_eq2v2f(x, y, f1, f2);				\
	_declare_true_eq2v1f(x, y, f1);					\
	_declare_true_eq2v1f(x, y, f2);					\
	_declare_true_eq1v2f(x, f1, f2);				\
	_declare_true_eq1v2f(y, f1, f2);				\
	_declare_true_eq1v1f(x, f1);					\
	_declare_true_eq1v1f(y, f1);					\
	_declare_true_eq1v1f(x, f2);					\
	_declare_true_eq1v1f(x, f2)


#if test_debug

#	define _declare_true_ne_impl(x, y, m_x, m_y)			\
	std::clog << "EXPECT_TRUE("#x"."#m_x"() "			\
		"!= "#y"."#m_y"());" << std::endl;			\
	EXPECT_TRUE(x.m_x() != y.m_y());				\
	std::clog << "EXPECT_FALSE("#x"."#m_x"() "			\
		"== "#y"."#m_y"());" << std::endl;			\
	EXPECT_FALSE(x.m_x() == y.m_y())

#else

#	define _declare_true_ne_impl(x, y, m_x, m_y)			\
	EXPECT_TRUE(x.m_x() != y.m_y());				\
	EXPECT_FALSE(x.m_x() == y.m_y())

#endif


#	define _declare_true_ne(x, y, m_x, m_y)				\
	_declare_true_ne_impl(x, y, m_x, m_y);				\
	_declare_true_ne_impl(y, x, m_y, m_x)

#	define declare_true_ne(x, y, m_x, m_y)				\
	_declare_true_ne(x, y, m_x, m_y)				\

#else
#	error declare_true_eq is already defined
#endif


TEST(basic, bounds_eqity)
{
	bolgenos_testing::forward_list<int> l;
	const auto& cref_l = l;

	EXPECT_TRUE(l.empty());
	EXPECT_TRUE(cref_l.empty());


	// some tests probable will be declared twice or more after
	// preprocessing but it is not bad
	declare_true_eq(l, cref_l, end, cend);
	declare_true_eq(l, cref_l, begin, cbegin);
	declare_true_eq(l, cref_l, before_begin, cbefore_begin);
	declare_true_eq(l, cref_l, begin, end);
	declare_true_eq(l, cref_l, begin, cend);
	declare_true_eq(l, cref_l, cbegin, end);
	declare_true_eq(l, cref_l, cbegin, cend);


	l.push_front(1);

	declare_true_ne(l, l, begin, end);
	declare_true_ne(cref_l, cref_l, begin, end);
	declare_true_ne(l, cref_l, begin, end);
	declare_true_ne(cref_l, l, begin, end);

	l.push_front(2);
}


TEST(basic, deallocation)
{
	auto list = new bolgenos_testing::forward_list<double>();

	list->push_front(1.0);
	list->push_front(2.0);
	list->push_front(3.0);
	list->push_front(4.0);

	delete list;
	SUCCEED();
}


TEST(iterator_read, first_element)
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


TEST(iterator_read, sequental_access)
{
	using list_type = bolgenos_testing::forward_list<int>;
	list_type l;
	const list_type& cref_l = l;

	for(int i = 0; i < 10; ++i) {
		l.push_front(i);
	}

	list_type::const_iterator	l_cbb =		l.cbefore_begin();
	list_type::iterator		l_bb =		l.before_begin();
	list_type::const_iterator	cref_l_cbb =	cref_l.cbefore_begin();
	list_type::const_iterator	cref_l_bb =	cref_l.before_begin();

	list_type::const_iterator	l_cb =		l.cbegin();
	list_type::iterator		l_b =		l.begin();
	list_type::const_iterator	cref_l_cb =	cref_l.cbegin();
	list_type::const_iterator	cref_l_b =	cref_l.begin();

	++l_cbb;
	++l_bb;
	++cref_l_cbb;
	++cref_l_bb;

	for(int i = 9; i >= 0; --i)
	{
		EXPECT_EQ(i, *l_cbb);
		EXPECT_EQ(i, *l_bb);
		EXPECT_EQ(i, *l_cb);
		EXPECT_EQ(i, *l_b);

		EXPECT_EQ(i, *cref_l_cbb);
		EXPECT_EQ(i, *cref_l_bb);
		EXPECT_EQ(i, *cref_l_cb);
		EXPECT_EQ(i, *cref_l_b);


		if (i >= 4) {
			++l_cb;
			++l_b;
			++cref_l_cb;
			++cref_l_b;
			++l_cbb;
			++l_bb;
			++cref_l_cbb;
			++cref_l_bb;
		} else {
			l_cb++;
			l_b++;
			cref_l_cb++;
			cref_l_b++;
			l_cbb++;
			l_bb++;
			cref_l_cbb++;
			cref_l_bb++;
		}
	}


	EXPECT_EQ(l.cend(), l_cbb);
	EXPECT_EQ(l.end(), l_bb);
	EXPECT_EQ(l.cend(), cref_l_cbb);
	EXPECT_EQ(l.cend(), cref_l_bb);
	EXPECT_EQ(l.cend(), l_cb);
	EXPECT_EQ(l.end(), l_b);
	EXPECT_EQ(l.cend(), cref_l_cb);
	EXPECT_EQ(l.cend(), cref_l_b);
}


TEST(iterator_write, rewrite_first)
{
	const int first_value = 10;
	const int second_value = 42;
	bolgenos_testing::forward_list<int> l;
	l.push_front(first_value);

	auto begin = l.begin();
	auto cbegin = l.cbegin();

	*begin = second_value;

	EXPECT_EQ(second_value, *l.begin());
	EXPECT_EQ(second_value, *begin);
	EXPECT_EQ(second_value, *cbegin);

	EXPECT_FALSE(l.empty());

	l.pop_front();

	EXPECT_TRUE(l.empty());
}
