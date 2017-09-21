#pragma once

#include "types.hpp"

namespace bolgenos { namespace mm {


class PageAlloc
{
public:
	const static pblk_t failed;
	pblk_t allocate_pages(std::size_t pages);
	pblk_t allocate_memory(std::size_t memsize);
	void deallocate();
};


} }
