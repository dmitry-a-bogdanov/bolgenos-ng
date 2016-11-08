#pragma once


namespace testing {

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

} // namespace testing
