#pragma once

namespace std
{


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


}
// namespace std
