#include "bitarray.hpp"

#include <bolgenos-ng/bitarray.hpp>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/ost.hpp>


void ost::test_bitarray() {
	void *mem = memory::alloc_pages(1);
	util::inplace::BitArray ba;
	const size_t array_size = 10;
	ba.initialize(mem, array_size);

	for (size_t idx = 0; idx != array_size; ++idx) {
		ba.set(idx, idx % 2);
	}

	for (size_t idx = 0; idx != array_size; ++idx)
	{
		OST_ASSERT(ba.get(idx) == idx % 2);
	}

	LOG_INFO(__func__ << ": ok");

	memory::free_pages(mem);
}
