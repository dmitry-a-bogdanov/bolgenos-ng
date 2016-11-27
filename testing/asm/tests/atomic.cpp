#include <bolgenos-ng/atomic.hpp>

#include <gtest/gtest.h>

TEST(atomic, store_and_load)
{
	atomic::atomic<uint32_t> a_v(42);
	EXPECT_EQ((uint32_t) 42, a_v);
	a_v.store(13);
	EXPECT_EQ((uint32_t) 13, a_v.load());
}


TEST(atomic, increment)
{
	atomic::atomic<uint32_t> a_v(42);
	EXPECT_EQ((uint32_t) 42, a_v);
	atomic::atomic<uint32_t> b_v = ++a_v;
	EXPECT_EQ((uint32_t) 43, a_v);
	EXPECT_EQ((uint32_t) 43, b_v);
	atomic::atomic<uint32_t> c_v(a_v++);
	EXPECT_EQ((uint32_t) 43, c_v);
	EXPECT_EQ((uint32_t) 44, a_v);
}


TEST(atomic, decrement)
{
	atomic::atomic<uint32_t> a_v(42);
	EXPECT_EQ((uint32_t) 42, a_v);
	atomic::atomic<uint32_t> b_v = --a_v;
	EXPECT_EQ((uint32_t) 41, a_v);
	EXPECT_EQ((uint32_t) 41, b_v);
	atomic::atomic<uint32_t> c_v(a_v--);
	EXPECT_EQ((uint32_t) 41, c_v);
	EXPECT_EQ((uint32_t) 40, a_v);
}
