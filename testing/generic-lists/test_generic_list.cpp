#include "generic_list.hpp"

#include <forward_list>
#include <iostream>

#include <gtest/gtest.h>

TEST(acts_like_std, add_and_remove_10)
{
	std::forward_list<int> s;
	testing::forward_list<int> t;

	if (s.empty() != t.empty()) {
		std::clog << "before loop:"
			<< " s.empty() == " << s.empty()
			<< " t.empty() == " << t.empty();
	}

	for (int i = 0; i < 10; ++i) {
		s.push_front(i); t.push_front(i);
		if (s.front() != t.front()) {
			std::clog << "i: " << i
				<< " s.front() == " << s.front()
				<< " t.front() == " << t.front();
		}
	}

	for(int i = 0; i < 10; ++i) {
		if (s.empty() != t.empty()) {
			std::clog << "i: " << i
				<< " s.empty() == " << s.empty()
				<< " t.empty() == " << t.empty();
		}
		if (!t.empty()) {
			s.pop_front(); t.pop_front();
		}
	}

	if (s.empty() != t.empty()) {
		std::clog << "after loop:"
			<< " s.empty() == " << s.empty()
			<< " t.empty() == " << t.empty();
	}

}
