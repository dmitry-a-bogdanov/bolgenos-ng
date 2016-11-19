#include <forward_list.hpp>


#include <gtest/gtest.h>

TEST(basic_functionality, add_and_remove_10)
{
	bolgenos_testing::forward_list<int> t;

	EXPECT_TRUE(t.empty());

	for (int i = 0; i < 10; ++i) {
		t.push_front(i);
		EXPECT_EQ(i, t.front());
		EXPECT_FALSE(t.empty());
	}

	for(int i = 9; i >= 0; --i) {
		EXPECT_FALSE(t.empty());
		t.pop_front();
	}

	EXPECT_TRUE(t.empty());
}



