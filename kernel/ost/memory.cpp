#include "memory.hpp"

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/slab.hpp>

#include <config.h>
#include <ost.h>

#ifdef OST_MEMORY
void ost::page_alloc_test() {
	char *allocated[5];
	allocated[0] = reinterpret_cast<char*>(memory::alloc_pages(1));
	allocated[1] = reinterpret_cast<char*>(memory::alloc_pages(2));
	allocated[2] = reinterpret_cast<char*>(memory::alloc_pages(3));
	allocated[3] = reinterpret_cast<char*>(memory::alloc_pages(2));
	memory::free_pages(allocated[1]);
	allocated[4] = reinterpret_cast<char*>(memory::alloc_pages(2));
	memory::free_pages(allocated[0]);
	memory::free_pages(allocated[2]);
	memory::free_pages(allocated[3]);
	memory::free_pages(allocated[4]);

	if (allocated[1] == allocated[0] + PAGE_SIZE &&
		allocated[2] == allocated[1] + PAGE_SIZE*2 &&
		allocated[3] == allocated[2] + PAGE_SIZE*3 &&
		allocated[4] == allocated[1]) {
		cio::cout << __func__ << ": ok" << cio::endl;
	} else {
		cio::cout << __func__ << ": fail" << cio::endl;
		for (int i = 0; i < 5; ++i) {
			cio::cout << "a[" << i << "]=" << allocated[i];
		}
		cio::cout << cio::endl;
		panic("FAILED TEST");
	}
}

void ost::slab_test() {
	struct slab_area test_slab(sizeof(long), 10);
	if (!test_slab.initialized()) {
		cio::cout	<< __func__
				<< ": slab initialization failure" << cio::endl;
		panic("FAILED TEST");
	}
	char *p[5];
	p[0] = reinterpret_cast<char*>(test_slab.allocate());
	p[1] = reinterpret_cast<char*>(test_slab.allocate());
	p[2] = reinterpret_cast<char*>(test_slab.allocate());
	p[3] = reinterpret_cast<char*>(test_slab.allocate());
	test_slab.deallocate(p[2]);
	p[4] = reinterpret_cast<char*>(test_slab.allocate());
	if (p[1] == p[0] + sizeof(long) &&
		p[2] == p[1] + sizeof(long) &&
		p[3] == p[2] + sizeof(long) &&
		p[4] == p[2]) {
		cio::cout << __func__ << ": ok" << cio::endl;
	} else {
		cio::cout << __func__ << ": fail" << cio::endl;
		for (int i = 0; i < 5; ++i) {
			cio::cout << "p[" << i << "]=" << p[i];
		}
		cio::cout << cio::endl;
		panic("FAILED TEST");
	}
}
#else
void ost::page_alloc_test() {
}
void ost::slab_test() {
}
#endif
