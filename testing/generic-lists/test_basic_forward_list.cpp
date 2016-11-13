#include "basic_forward_list.hpp"


TEST(has_member_next, positive)
{
	struct positive
	{
		positive* next;
	};


	EXPECT_EQ(true, testing::____impl::has_member_next<positive>::value);
}





TEST(has_member_next, negative)
{
	struct negative
	{
		// nothing
	};


	EXPECT_EQ(false, testing::____impl::has_member_next<negative>::value);
}




TEST(has_member_next, has_but_private)
{
	struct has_but_private
	{
	private:
		has_but_private *next;
	};


	EXPECT_EQ(false,
		testing::____impl::has_member_next<has_but_private>::value);
}
