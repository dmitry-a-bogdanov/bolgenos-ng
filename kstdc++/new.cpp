#include <new>

#include <bolgenos-ng/memory.hpp>

const std::nothrow_t std::nothrow;


void *operator new(std::size_t size)
{
	return bolgenos::kmalloc(size);
}


void *operator new[](std::size_t size)
{
	return bolgenos::kmalloc(size);
}


void operator delete(void *p) noexcept
{
	bolgenos::kfree(p);
}

void operator delete(void *p, std::size_t) noexcept
{
	bolgenos::kfree(p);
}


void operator delete[](void *p, std::size_t) noexcept
{
	bolgenos::kfree(p);
}

void operator delete[](void *p) noexcept
{
	bolgenos::kfree(p);
}

