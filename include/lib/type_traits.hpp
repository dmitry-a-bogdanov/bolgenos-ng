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


} // namespace lib
