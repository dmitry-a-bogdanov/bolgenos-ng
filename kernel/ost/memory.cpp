#include "memory.hpp"

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/slab.hpp>

#include <config.h>
#include <ost.h>


#ifdef OST_MEMORY
void ost::page_alloc_test() {
	void *p[5];
	p[0] = memory::alloc_pages(1);
	p[1] = memory::alloc_pages(2);
	p[2] = memory::alloc_pages(3);
	p[3] = memory::alloc_pages(2);
	memory::free_pages(p[1]);
	p[4] = memory::alloc_pages(2);
	memory::free_pages(p[0]);
	memory::free_pages(p[2]);
	memory::free_pages(p[3]);
	memory::free_pages(p[4]);

	if (reinterpret_cast<char *>(p[1])
			== reinterpret_cast<char *>(p[0]) + PAGE_SIZE &&
		reinterpret_cast<char *>(p[2])
			== reinterpret_cast<char *>(p[1]) + PAGE_SIZE*2 &&
		reinterpret_cast<char *>(p[3])
			== reinterpret_cast<char *>(p[2]) + PAGE_SIZE*3 &&
		reinterpret_cast<char *>(p[4])
			== reinterpret_cast<char *>(p[1])) {
		cio::cinfo << __func__ << ": ok" << cio::endl;
	} else {
		cio::cerr << __func__ << ": fail" << cio::endl;
		for (int i = 0; i < 5; ++i) {
			cio::cerr << "a[" << i << "]=" << p[i] << " ";
		}
		cio::cerr << cio::endl;
		panic("FAILED TEST");
	}
}

void ost::slab_test() {
	struct slab_area test_slab(sizeof(long), 10);
	if (!test_slab.initialized()) {
		cio::cerr	<< __func__
				<< ": slab initialization failure" << cio::endl;
		panic("FAILED TEST");
	}
	void *p[5];
	p[0] = test_slab.allocate();
	p[1] = test_slab.allocate();
	p[2] = test_slab.allocate();
	p[3] = test_slab.allocate();
	test_slab.deallocate(p[2]);
	p[4] = test_slab.allocate();
	if (reinterpret_cast<char *>(p[1])
			== reinterpret_cast<char *>(p[0]) + sizeof(long) &&
		reinterpret_cast<char *>(p[2])
			== reinterpret_cast<char *>(p[1]) + sizeof(long) &&
		reinterpret_cast<char *>(p[3])
			== reinterpret_cast<char *>(p[2]) + sizeof(long) &&
		reinterpret_cast<char *>(p[4])
			== reinterpret_cast<char *>(p[2])) {
		cio::cinfo << __func__ << ": ok" << cio::endl;
	} else {
		cio::cerr << __func__ << ": fail" << cio::endl;
		for (int i = 0; i < 5; ++i) {
			cio::cerr << "p[" << i << "]=" << p[i];
		}
		cio::cerr << cio::endl;
		panic("FAILED TEST");
	}
}
#else
void ost::page_alloc_test() {
}
void ost::slab_test() {
}
#endif
