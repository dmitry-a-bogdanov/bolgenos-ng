#pragma once

#include "cstddef.hpp"
#include "utility.hpp"
#include <type_traits.hpp>

namespace lib {


template<class T>
class default_allocator
{
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using size_type = size_t;

	template<class Other>
	struct rebind
	{
		using other = default_allocator<Other>;
	};

	pointer allocate(size_type n)
	{
		return new T[n];
	}

	void deallocate(pointer p, size_type n __attribute__((unused)))
	{
		delete[] p;
	}
}; // class default_allocator


template<typename T>
struct default_delete
{
public:
	constexpr default_delete() = default;

	template <typename U>
	default_delete(const default_delete<U>&) {}

	void operator()(T* ptr)
	{
		delete ptr;
	}
};


template<typename T>
struct default_delete<T[]>
{
public:
	constexpr default_delete() = default;

	template <typename U>
	default_delete(const default_delete<U[]>&) {}

	void operator()(T* ptr)
	{
		delete[] ptr;
	}
};



template<typename T, typename Deleter = default_delete<T>>
class unique_ptr
{
public:
	using pointer = T*;
	using deleter_type = Deleter;


	constexpr unique_ptr() noexcept = default;


	constexpr unique_ptr(nullptr_t) noexcept
		: _object{nullptr} {}


	explicit unique_ptr(pointer p) noexcept
		: _object{p} {}


	unique_ptr(const unique_ptr&) = delete;


	unique_ptr(unique_ptr&& other) noexcept
	{
		swap(_object, other._object);
		swap(_deleter, other._deleter);
	}


	~unique_ptr()
	{
		if (_object)
		{
			get_deleter()(get());
		}
	}


	unique_ptr& operator=(const unique_ptr&) = delete;


	unique_ptr& operator=(unique_ptr&& other)
	{
		reset(other.release());
		_deleter = move(other.get_deleter());
		return *this;
	}


	pointer get() const noexcept
	{
		return _object;
	}


	void reset(pointer ptr = pointer()) noexcept
	{
		auto old_ptr = _object;
		_object = ptr;
		if (old_ptr)
		{
			get_deleter()(old_ptr);
		}
	}


	pointer release() noexcept
	{
		pointer old_ptr = _object;
		_object = nullptr;
		return old_ptr;
	}


	deleter_type& get_deleter() noexcept
	{
		return _deleter;
	}


	const deleter_type& get_deleter() const noexcept
	{
		return _deleter;
	}


	pointer operator->() const noexcept
	{
		return get();
	}

private:
	pointer _object{ nullptr };
	deleter_type _deleter{};
};


template<typename T, typename Deleter >
class unique_ptr<T[], Deleter>
{
public:
	using pointer = T*;
	using deleter_type = Deleter;


	constexpr unique_ptr() noexcept = default;


	constexpr unique_ptr(nullptr_t) noexcept
		: _object{nullptr} {}


	explicit unique_ptr(pointer p) noexcept
		: _object{p} {}


	unique_ptr(const unique_ptr&) = delete;


	unique_ptr(unique_ptr&& other) noexcept
	{
		swap(_object, other._object);
		swap(_deleter, other._deleter);
	}


	~unique_ptr()
	{
		if (_object)
		{
			get_deleter()(get());
		}
	}


	unique_ptr& operator=(const unique_ptr&) = delete;


	unique_ptr& operator=(unique_ptr&& other)
	{
		reset(other.release());
		_deleter = move(other.get_deleter());
		return *this;
	}


	pointer get() const noexcept
	{
		return _object;
	}


	void reset(pointer ptr = pointer()) noexcept
	{
		auto old_ptr = _object;
		_object = ptr;
		if (old_ptr)
		{
			get_deleter()(old_ptr);
		}
	}


	pointer release() noexcept
	{
		pointer old_ptr = _object;
		_object = nullptr;
		return old_ptr;
	}


	deleter_type& get_deleter() noexcept
	{
		return _deleter;
	}


	const deleter_type& get_deleter() const noexcept
	{
		return _deleter;
	}


	pointer operator->() const noexcept
	{
		return get();
	}

private:
	pointer _object{ nullptr };
	deleter_type _deleter{};
};


template<class T, class... Args>
unique_ptr<T> make_unique(Args&& ...args)
{
	return unique_ptr<T>(new T(forward<Args>(args)...));
}

template<class T>
unique_ptr<T> make_unique(lib::size_t size) {
	return unique_ptr<T>(new typename lib::remove_extent_t<T>[size]);
}


} // namespace lib

