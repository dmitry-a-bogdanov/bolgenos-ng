#pragma once

#include <cstddef>
#include <utility>

#include "default_delete.hpp"

namespace std {


template<typename T, typename Deleter = std::default_delete<T>>
class unique_ptr
{
public:
	using pointer = T*;
	using deleter_type = Deleter;


	constexpr unique_ptr() noexcept = default;


	constexpr unique_ptr(nullptr_t) noexcept
		: _object{nullptr}
	{
	}


	explicit unique_ptr(pointer p) noexcept
		: _object{p}
	{
	}


	unique_ptr(pointer p, const deleter_type& d) noexcept
		: _object{p}, _deleter{d}
	{
	}


	template<class OtherPointerType, class OtherDeleterType>
	unique_ptr(unique_ptr<OtherPointerType, OtherDeleterType>&& other)
	{
		_object = other.release();
		_deleter = other.get_deleter();
	}


	unique_ptr(const unique_ptr&) = delete;


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
		_deleter = std::move(other.get_deleter());
		return *this;
	}


	template<class OtherPointerType, class OtherPointerDeleter>
	unique_ptr& operator =(unique_ptr<OtherPointerType, OtherPointerDeleter>&& other)
	{
		reset(other.release());
		get_deleter() = std::move(other.get_deleter());
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
unique_ptr<T> make_unique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


}
