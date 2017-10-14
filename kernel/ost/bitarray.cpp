#include <bolgenos-ng/bitarray.hpp>

#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/ost.hpp>


TEST(BitArray, basic_test)
{
	void *mem = bolgenos::alloc_pages(1);
	util::inplace::BitArray ba;
	const size_t array_size = 10;
	ba.initialize(mem, array_size);

	for (size_t idx = 0; idx != array_size; ++idx)
	{
		ba.set(idx, idx % 2);
	}

	for (size_t idx = 0; idx != array_size; ++idx)
	{
		OST_ASSERT_EQ(ba.get(idx), idx % 2);
	}

	bolgenos::free_pages(mem);
}
