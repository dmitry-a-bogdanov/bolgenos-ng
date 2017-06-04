#pragma once

#include <bolgenos-ng/asm.hpp>

#include "type_traits.hpp"


namespace lib {


template<class T>
struct atomic
{
public:
	atomic() = default;


	inline
	constexpr atomic(T value) :
		_value(value)
	{
	}

	atomic(const atomic& other) = delete;


	inline
	T load() const
	{
		T local;
		x86::load<T>(_value, local);
		return local;
	}


	inline
	explicit operator T() const
	{
		return load();
	}


	inline
	void store(T value)
	{
		x86::store<T>(value, _value);
	}


	inline
	T exchange(T value)
	{
		x86::exchange<T>(_value, value);
		return value;
	}

	inline
	T fetch_add(T increment)
	{
		return x86::fetch_add<T>(_value, increment);
	}


	inline
	T fetch_sub(T decrement)
	{
		return fetch_add(-decrement);
	}


	inline
	T operator++()
	{
		T value = fetch_add(1);
		++value;
		return value;
	}


	inline
	T operator++(int)
	{
		T tmp = fetch_add(1);
		return tmp;
	}


	inline
	T operator--()
	{
		T value = fetch_sub(1);
		--value;
		return value;
	}


	inline
	T operator--(int)
	{
		T tmp = fetch_sub(1);
		return tmp;
	}

private:
	T _value{0};
};


} // namespace atomic
