#include <bolgenos-ng/ost.hpp>

#include <bolgenos-ng/cout.hpp>

#include <ost.h>
#include "memory.hpp"

#ifdef OST_ENABLE_TESTS
void ost::run() {
	cio::cout << "OST: running..." << cio::endl;
	ost::page_alloc_test();
	ost::slab_test();
	cio::cout << "OST: finished" << cio::endl;;
}
#else
void ost::run() {
}
#endif
