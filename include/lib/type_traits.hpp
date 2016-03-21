#pragma once


namespace lib {

/// \brief Compile-time constant of integral type.
///
/// The class is intended to provide functionality of compile time constants
/// of integral types.
/// \tparam ValueType type of integral constant
/// \tparam Value value of integral constant
template<class ValueType, ValueType Value>
struct integral_constant {


	/// Alias for ValueType
	using value_type = ValueType;


	/// Alias for Value
	static constexpr value_type value = Value;


	/// Alias for own type.
	using type = integral_constant<value_type, value>;


	/// Returns value of integral constant.
	constexpr operator value_type() const noexcept { return value; }
};


using true_type = lib::integral_constant<bool, true>;


using false_type = lib::integral_constant<bool, false>;


template<typename Type>
struct is_unsigned: false_type {};


template<>
struct is_unsigned<unsigned long long>: true_type {};


template<>
struct is_unsigned<unsigned long>: true_type {};


template<>
struct is_unsigned<unsigned int>: true_type {};


template<>
struct is_unsigned<unsigned short>: true_type {};


template<>
struct is_unsigned<unsigned char>: true_type {};


template<typename Type>
struct is_signed: false_type {};


template<>
struct is_signed<long long>: true_type {};


template<>
struct is_signed<long>: true_type {};


template<>
struct is_signed<int>: true_type {};


template<>
struct is_signed<short>: true_type {};


template<>
struct is_signed<char>: true_type {};


template<bool Cond, class T = void>
struct enable_if {
	// type is not declared in this class
};


template<class T>
struct enable_if<true, T> {
	using type = T;
};


template<typename T>
struct make_unsigned {
	using type = typename enable_if<is_unsigned<T>::value, T>::type;
};


template<>
struct make_unsigned<char> {
	using type = unsigned char;
};


template<>
struct make_unsigned<short> {
	using type = unsigned short;
};


template<>
struct make_unsigned<int> {
	using type = unsigned int;
};


template<>
struct make_unsigned<long> {
	using type = unsigned long;
};


template<>
struct make_unsigned<long long> {
	using type = unsigned long long;
};



} // namespace lib
