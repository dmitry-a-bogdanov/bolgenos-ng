#include <lib/atomic.hpp>

#include <gtest/gtest.h>
#include <limits>


template<typename T>
class atomic_test: public ::testing::Test
{
};

TYPED_TEST_CASE_P(atomic_test);

TYPED_TEST_P(atomic_test, store_and_load)
{
	using limits = std::numeric_limits<TypeParam>;
	const TypeParam first = limits::max() / 2 - 1;
	const TypeParam second = first / 2;

	atomic::atomic<TypeParam> a_v(first);
	EXPECT_EQ(first, static_cast<TypeParam>(a_v));

	a_v.store(second);
	EXPECT_EQ(second, a_v.load());
}


TYPED_TEST_P(atomic_test, increment)
{
	using limits = std::numeric_limits<TypeParam>;
	const TypeParam first = limits::max() / 2 - 1;

	atomic::atomic<TypeParam> a_v(first);
	EXPECT_EQ(first, static_cast<TypeParam>(a_v));
	atomic::atomic<TypeParam> b_v(static_cast<TypeParam>(++a_v));
	EXPECT_EQ(first + 1, static_cast<TypeParam>(a_v));
	EXPECT_EQ(first + 1, static_cast<TypeParam>(b_v));
	atomic::atomic<TypeParam> c_v(static_cast<TypeParam>(a_v++));
	EXPECT_EQ(first + 1, static_cast<TypeParam>(c_v));
	EXPECT_EQ(first + 2, static_cast<TypeParam>(a_v));
}


TYPED_TEST_P(atomic_test, decrement)
{
	using limits = std::numeric_limits<TypeParam>;
	const TypeParam first = limits::max() / 2 - 1;

	atomic::atomic<TypeParam> a_v(first);
	EXPECT_EQ(first, static_cast<TypeParam>(a_v));

	atomic::atomic<TypeParam> b_v(static_cast<TypeParam>(--a_v));
	EXPECT_EQ(first - 1, static_cast<TypeParam>(a_v));
	EXPECT_EQ(first - 1, static_cast<TypeParam>(b_v));
	atomic::atomic<TypeParam> c_v(a_v--);
	EXPECT_EQ(first - 1, static_cast<TypeParam>(c_v));
	EXPECT_EQ(first - 2, static_cast<TypeParam>(a_v));
}


TYPED_TEST_P(atomic_test, inc_dec_bounds)
{
	using limits = std::numeric_limits<TypeParam>;

	atomic::atomic<TypeParam> a_v(limits::max());
	// it looks like UB for signed type for both C/C++, but we are working
	// with registers.
	++a_v;
	EXPECT_EQ(limits::min(), a_v.load());
	--a_v;
	EXPECT_EQ(limits::max(), a_v.load());
	a_v++;
	EXPECT_EQ(limits::min(), a_v.load());
	a_v--;
	EXPECT_EQ(limits::max(), a_v.load());
}


using atomic_test_types = ::testing::Types<
	int8_t, uint8_t, char,
	int16_t, uint16_t,
	int32_t, uint32_t
>;


REGISTER_TYPED_TEST_CASE_P(atomic_test,
	store_and_load,
	increment,
	decrement,
	inc_dec_bounds);

INSTANTIATE_TYPED_TEST_CASE_P(, atomic_test, atomic_test_types);

