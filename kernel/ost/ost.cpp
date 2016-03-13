#include <bolgenos-ng/ost.hpp>

#include <bolgenos-ng/cout.hpp>

#include <ost.h>
#include "memory.hpp"
#include "bitarray.hpp"

#ifdef OST_ENABLE_TESTS
void ost::run() {
	cio::cnotice << "OST: running..." << cio::endl;
	ost::page_alloc_test();
	ost::free_list_test();
	ost::slab_test();
	ost::buddy_allocator_test();
	ost::test_bitarray();
	ost::mallocator_test();
	cio::cnotice << "OST: finished" << cio::endl;;
}
#else
void ost::run() {
}
#endif
