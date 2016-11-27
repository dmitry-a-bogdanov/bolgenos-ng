#pragma once

namespace atomic {

template<class T>
struct atomic
{
public:
	constexpr atomic(T value) :
		value_(value)
	{
	}


	atomic(const atomic& other)
	{
		value_ = other.load();
	}


	T load() const
	{
		T local;
		asm("movl %1, %0\n":"=r"(local):"m"(value_));
		return local;
	}


	operator T() const
	{
		return load();
	}


	void store(T value)
	{
		asm("movl %1, %0\n":"=m"(value_):"r"(value));
	}


	atomic& operator++()
	{
		asm("lock incl %0\n"::"m"(value_));
		return *this;
	}


	atomic operator++(int)
	{
		atomic tmp(*this);
		asm("lock incl %0\n"::"m"(value_));
		return tmp;
	}


private:
	T value_;
};


} // namespace atomic
