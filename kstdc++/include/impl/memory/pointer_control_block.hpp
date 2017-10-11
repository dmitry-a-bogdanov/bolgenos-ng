#pragma once

#include <cstddef>
#include <memory>

#if 0
namespace std { namespace _details {


// FIXME: make me atomic
template<class T, class Deleter = std::default_delete>
class pointer_control_block
{
public:
	void release(){}
	void shared_release()
	{
		--_shared_count;
		if (_shared_count == 0)
		{
			if (_object != nullptr)
			{
				deleter(_object);
				_object = nullptr;
			}
		}
	}

	std::size_t count()
	{
		return _shared_count + _weak_count;
	}

private:

	Deleter deleter;
	std::size_t _shared_count;
	std::size_t _weak_count;
	T *_object;
};

}}

#endif
