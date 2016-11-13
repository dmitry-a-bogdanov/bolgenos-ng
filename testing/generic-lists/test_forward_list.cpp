#include "forward_list.hpp"

#include <forward_list>

#include <gtest/gtest.h>

TEST(acts_like_std, add_and_remove_10)
{
	std::forward_list<int> s;
	testing::forward_list<int> t;

	ASSERT_EQ(s.empty(), t.empty()) << " before loop";

	for (int i = 0; i < 10; ++i) {
		s.push_front(i); t.push_front(i);
		ASSERT_EQ(s.front(), t.front()) << " when i = " << i;
	}

	for(int i = 0; i < 10; ++i) {
		ASSERT_EQ(s.empty(), t.empty()) << " when i = " << i;
		if (!t.empty()) {
			s.pop_front(); t.pop_front();
		}
	}

	ASSERT_EQ(s.empty(), t.empty()) << " after loop";
}

