#include <new.hpp>

void *operator new(size_t, void *address)
{
	return address;
}
