#include <bolgenos-ng/error.h>
#include <bitarray.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/ost.hpp>


LOCAL_LOGGER("bitarray_test", lib::LogLevel::INFO);

TEST(BitArray, test) {
	void *mem = memory::alloc_pages(1);
	util::inplace::BitArray ba;
	const size_t array_size = 10;
	ba.initialize(mem, array_size);

	for (size_t idx = 0; idx != array_size; ++idx) {
		ba.set(idx, idx % 2);
	}

	for (size_t idx = 0; idx != array_size; ++idx) {
		if (ba.get(idx) != idx % 2) {
			LOG_ERROR << __func__ << ": bug on idx=" << idx
				<< " " << ba.get(idx) << " vs. "
				<< idx % 2 << lib::endl;
			panic("Failed Test!");
		}
	}
	memory::free_pages(mem);
}
