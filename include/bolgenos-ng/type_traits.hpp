#pragma once


template<class ValueType, ValueType Value>
struct integral_constant {
	static constexpr ValueType value = Value;
	typedef ValueType value_type;
	typedef integral_constant<ValueType, Value> type;
	constexpr operator ValueType() const noexcept { return Value; }
};
