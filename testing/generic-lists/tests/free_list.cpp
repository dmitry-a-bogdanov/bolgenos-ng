#include <lib/free_list.hpp>

#include <gtest/gtest.h>

TEST(basic, push_and_pop_10)
{
	const int test_size = 10;
	const size_t alloc_size = 16;

	lib::free_list l;

	char* allocated[test_size];

	ASSERT_GE(alloc_size, lib::free_list::min_size::value);
	ASSERT_LE(sizeof(void *), lib::free_list::min_size::value);

	for (int i = 0; i < test_size - 2; ++i) {
		allocated[i] = new char[alloc_size];
		l.push_front(allocated[i]);
		EXPECT_EQ(allocated[i], l.front()) << "i = " << i;
	}


	allocated[test_size - 2] = new char[alloc_size];
	allocated[test_size - 1] = new char[alloc_size];
	l.push_after(l.before_begin(), allocated[test_size - 1]);
	l.push_after(l.begin(), allocated[test_size - 2]);

	EXPECT_EQ(allocated[test_size - 1], *l.begin());
	EXPECT_EQ(allocated[test_size - 2], *++l.begin());

	{
		int i = 0;
		for (lib::free_list::iterator iter = l.begin();
				iter != l.end(); ++iter) {
			EXPECT_EQ(allocated[test_size - 1 - i], *iter);
			++i;
		}
		EXPECT_EQ(test_size, i);
	}


	{
		int i = 0;
		for (lib::free_list::const_iterator iter
				= l.cbegin(); iter != l.cend(); ++iter) {
			EXPECT_EQ(allocated[test_size - 1 - i], *iter);
			++i;
		}
		EXPECT_EQ(test_size, i);
	}


	for (int i = test_size - 1; i >= 2; --i) {
		EXPECT_EQ(allocated[i], l.front());
		l.pop_front();
		delete[] allocated[i];
	}

	EXPECT_EQ(allocated[1], *l.begin());
	EXPECT_EQ(allocated[0], *++l.begin());

	l.erase_after(l.begin());
	EXPECT_EQ(allocated[1], *l.begin());
	EXPECT_EQ(l.cend(), ++l.begin());

	delete[] allocated[0];

	EXPECT_FALSE(l.empty());
	l.erase_after(l.before_begin());
	EXPECT_TRUE(l.empty());

	delete[] allocated[1];
}


