#pragma once

#include "type_traits.hpp"


namespace atomic {


template<typename T>
struct atomic_struct
{
	inline
	constexpr atomic_struct() = default;

	inline
	constexpr atomic_struct(T value) :
		value_(value)
	{
	}

	T value_;
};


template<typename T, class Enabled = void>
struct __atomic_base;



template<typename T>
struct __atomic_base<T, typename lib::enable_if<(sizeof(T) == 4)>::type> :
	private atomic_struct<T>
{
	using base_type = atomic_struct<T>;

	inline
	constexpr __atomic_base(T value) :
		base_type(value)
	{
	}


	inline
	T load() const
	{
		T local;
		asm("movl %1, %0\n":"=r"(local):"m"(base_type::value_));
		return local;
	}


	inline
	void store(T value)
	{
		asm("movl %1, %0\n":"=m"(base_type::value_):"r"(value));
	}


	inline
	T fetch_add(T increment)
	{
		asm("lock; xaddl %k0, %1"
			: "+r"(increment), "+m"(base_type::value_)
			:
			: "memory");
		return increment;
	}
};


template<typename T>
struct __atomic_base<T, typename lib::enable_if<(sizeof(T) == 2)>::type> :
	private atomic_struct<T>
{
	using base_type = atomic_struct<T>;

	inline
	constexpr __atomic_base(T value) :
		base_type(value)
	{
	}


	inline
	T load() const
	{
		T local;
		asm("movw %1, %w0\n":"=r"(local):"m"(base_type::value_));
		return local;
	}


	inline
	void store(T value)
	{
		asm("movw %w1, %0\n":"=m"(base_type::value_):"r"(value));
	}


	inline
	T fetch_add(T increment)
	{
		asm("lock; xaddw %w0, %1"
			: "+r"(increment), "+m"(base_type::value_)
			:
			: "memory");
		return increment;
	}
};


template<typename T>
struct __atomic_base<T, typename lib::enable_if<(sizeof(T) == 1)>::type> :
	private atomic_struct<T>
{
	using base_type = atomic_struct<T>;

	inline
	constexpr __atomic_base(T value) :
		base_type(value)
	{
	}


	inline
	T load() const
	{
		T local;
		asm("movb %1, %b0\n":"=r"(local):"m"(base_type::value_));
		return local;
	}


	inline
	void store(T value)
	{
		asm("movb %b1, %0\n":"=m"(base_type::value_):"r"(value));
	}


	inline
	T fetch_add(T increment)
	{
		asm("lock; xaddb %b0, %1"
			: "+r"(increment), "+m"(base_type::value_)
			:
			: "memory");
		return increment;
	}
};


template<typename T>
struct atomic_base :
	public __atomic_base<T>
{
	using specific_base = __atomic_base<T>;

	inline
	constexpr atomic_base(T value) :
		specific_base(value)
	{
	}


	inline
	T fetch_sub(T decrement)
	{
		return specific_base::fetch_add(-decrement);
	}

};


template<class T>
struct atomic :
	private atomic_base<T>
{
	using base_type = atomic_base<T>;
public:
	atomic() = default;


	inline
	constexpr atomic(T value) :
		base_type(value)
	{
	}


	atomic(const atomic& other) = delete;


	inline
	T load() const
	{
		return base_type::load();
	}


	inline
	explicit operator T() const
	{
		return load();
	}


	inline
	void store(T value)
	{
		base_type::store(value);
	}


	inline
	T fetch_add(T value)
	{
		return base_type::fetch_add(value);
	}


	inline
	T fetch_sub(T value)
	{
		return base_type::fetch_sub(value);
	}


	inline
	T operator++()
	{
		T value = base_type::fetch_add(static_cast<T>(1));
		++value;
		return value;
	}


	inline
	T operator++(int)
	{
		T tmp = base_type::fetch_add(static_cast<T>(1));
		return tmp;
	}


	inline
	T operator--()
	{
		T value = base_type::fetch_sub(static_cast<T>(1));
		--value;
		return value;
	}


	inline
	T operator--(int)
	{
		T tmp = base_type::fetch_sub(static_cast<T>(1));
		return tmp;
	}
};


} // namespace atomic
