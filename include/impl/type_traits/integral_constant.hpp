#pragma once


namespace lib {


template<class T, T Value>
struct integral_constant {
	using value_type = T;

	static constexpr value_type value = Value;

	using type = integral_constant<value_type, value>;

	constexpr operator value_type() const noexcept { return value; }

	constexpr value_type operator()() const noexcept { return value; }
};


template<class T, T Value>
constexpr typename integral_constant<T, Value>::value_type
integral_constant<T, Value>::value;


using true_type = integral_constant<bool, true>;


using false_type = integral_constant<bool, false>;


} // namespace lib
