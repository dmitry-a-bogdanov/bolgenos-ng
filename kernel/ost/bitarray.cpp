#include "bitarray.hpp"

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/bitarray>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/cout.hpp>

void ost::test_bitarray() {
	void *mem = memory::alloc_pages(1);
	util::inplace::BitArray ba;
	const size_t array_size = 10;
	ba.initialize(mem, array_size);

	for (size_t idx = 0; idx != array_size; ++idx) {
		ba.set(idx, idx % 2);
	}

	for (size_t idx = 0; idx != array_size; ++idx) {
		if (ba.get(idx) != idx % 2) {
			cio::cerr << __func__ << ": bug on idx=" << idx
				<< " " << ba.get(idx) << " vs. "
				<< idx % 2 << cio::endl;
			panic("Failed Test!");
		}
	}

	cio::cinfo << __func__ << ": ok" << cio::endl;

	memory::free_pages(mem);
}
