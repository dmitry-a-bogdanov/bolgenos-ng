#pragma once

#include <cstddef>

namespace bolgenos { namespace mm {


struct _blk_data_t
{
	void * const address;
	std::size_t const size;
};


struct blk_t: private _blk_data_t
{
	using _blk_data_t::address;
	using _blk_data_t::size;
};


struct pblk_t: private _blk_data_t
{
	using _blk_data_t::address;
	using _blk_data_t::size;
};



} }
