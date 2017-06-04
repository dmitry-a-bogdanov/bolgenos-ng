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

	lib::atomic<TypeParam> a_v(first);
	EXPECT_EQ(first, static_cast<TypeParam>(a_v));

	a_v.store(second);
	EXPECT_EQ(second, a_v.load());
}


TYPED_TEST_P(atomic_test, increment)
{
	using limits = std::numeric_limits<TypeParam>;
	const TypeParam first = limits::max() / 2 - 1;

	lib::atomic<TypeParam> a_v(first);
	EXPECT_EQ(first, static_cast<TypeParam>(a_v));
	lib::atomic<TypeParam> b_v(static_cast<TypeParam>(++a_v));
	EXPECT_EQ(first + 1, static_cast<TypeParam>(a_v));
	EXPECT_EQ(first + 1, static_cast<TypeParam>(b_v));
	lib::atomic<TypeParam> c_v(static_cast<TypeParam>(a_v++));
	EXPECT_EQ(first + 1, static_cast<TypeParam>(c_v));
	EXPECT_EQ(first + 2, static_cast<TypeParam>(a_v));
}


TYPED_TEST_P(atomic_test, decrement)
{
	using limits = std::numeric_limits<TypeParam>;
	const TypeParam first = limits::max() / 2 - 1;

	lib::atomic<TypeParam> a_v(first);
	EXPECT_EQ(first, static_cast<TypeParam>(a_v));

	lib::atomic<TypeParam> b_v(static_cast<TypeParam>(--a_v));
	EXPECT_EQ(first - 1, static_cast<TypeParam>(a_v));
	EXPECT_EQ(first - 1, static_cast<TypeParam>(b_v));
	lib::atomic<TypeParam> c_v(a_v--);
	EXPECT_EQ(first - 1, static_cast<TypeParam>(c_v));
	EXPECT_EQ(first - 2, static_cast<TypeParam>(a_v));
}


TYPED_TEST_P(atomic_test, inc_dec_bounds)
{
	using limits = std::numeric_limits<TypeParam>;

	lib::atomic<TypeParam> a_v(limits::max());
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


TYPED_TEST_P(atomic_test, exchange_min_max)
{
	using Num = TypeParam;
	using limits = std::numeric_limits<Num>;

	const Num old_val{limits::max()};
	const Num middle_val{limits::min()};
	const Num new_val{limits::max()};

	lib::atomic<Num> a_v(old_val);

	EXPECT_EQ(old_val, a_v.load());
	EXPECT_EQ(old_val, a_v.exchange(middle_val));
	EXPECT_EQ(middle_val, a_v.load());
	EXPECT_EQ(middle_val, a_v.exchange(new_val));
	EXPECT_EQ(new_val, a_v.load());
}


TYPED_TEST_P(atomic_test, exchange_100_50)
{
	using Num = TypeParam;

	const Num old_val{100};
	const Num new_val{50};

	lib::atomic<Num> a_v(old_val);

	EXPECT_EQ(old_val, a_v.load());
	EXPECT_EQ(old_val, a_v.exchange(new_val));
	EXPECT_EQ(new_val, a_v.load());
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
	inc_dec_bounds,
	exchange_100_50,
	exchange_min_max);

INSTANTIATE_TYPED_TEST_CASE_P(, atomic_test, atomic_test_types);

