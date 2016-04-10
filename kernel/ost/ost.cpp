#include <bolgenos-ng/ost.hpp>

#include <lib/ostream.hpp>

#include <ost.h>
#include "memory.hpp"
#include "bitarray.hpp"

#ifdef OST_ENABLE_TESTS
void ost::run() {
	lib::cnotice << "OST: running..." << lib::endl;
	ost::page_alloc_test();
	ost::free_list_test();
	ost::slab_test();
	ost::buddy_allocator_test();
	ost::test_bitarray();
	ost::mallocator_test();
	lib::cnotice << "OST: finished" << lib::endl;;
}
#else
void ost::run() {
}
#endif
