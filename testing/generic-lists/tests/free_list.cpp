#include <free_list.hpp>


TEST(simple_free_list, push_and_pop_10)
{
	const int test_size = 10;

	testing::simple_free_list l;

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
