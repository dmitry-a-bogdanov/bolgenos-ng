#include "types.hpp"

namespace bolgenos { namespace mm {


class PageAllocatorBase
{
public:
	virtual pblk_t allocate(std::size_t pages) = 0;
	virtual void deallocate(const pblk_t&) = 0;
	virtual ~PageAllocatorBase() = default;
};



class AllocatorBase
{
	virtual blk_t allocate(std::size_t memsize) = 0;
	virtual blk_t deallocate(const blk_t&) = 0;
	virtual bool owns(const blk_t&) = 0;
};



}}
