#include "memory.hpp"

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/slab.hpp>

#include "../free_list.hpp"
#include "../buddy_allocator.hpp"
#include "../mallocator.hpp"

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

	void *q[5];
	q[0] = memory::alloc_pages(1);
	q[1] = memory::alloc_pages(2);
	q[2] = memory::alloc_pages(3);
	q[3] = memory::alloc_pages(2);
	memory::free_pages(q[1]);
	q[4] = memory::alloc_pages(2);

	for (size_t i = 0; i != 5; ++i) {
		if (p[i] != q[i]) {
			cio::cout << i << ": " << p[i] << " vs " << q[i] << cio::endl;
			panic("Failed test!");
		}
	}

	cio::cinfo << __func__ << ": ok" << cio::endl;
}

void ost::slab_test() {
	memory::allocators::SlabAllocator test_slab(sizeof(long), 10);
	if (!test_slab.is_initialized()) {
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


namespace {

void free_list_test__small_order__even() {
	void *pages = memory::alloc_pages(128);

	if (!pages) {
		cio::cerr << __func__ << ": allocation failed!" << cio::endl;
		panic("FAILED TEST");
	}

	memory::page_frame_t *first_address =
		reinterpret_cast<memory::page_frame_t *>(
			memory::align_up<PAGE_SIZE*2>(pages));
	memory::page_frame_t *second_address =
		first_address + 1;

	memory::page_frame_t *third_address =
		second_address + 1;

	memory::allocators::FreeList fl;
	if (!fl.initialize(0)) {
		cio::cerr << __func__ << ": initialization failed!" << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(first_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(second_address) != first_address) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(third_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.get() != third_address) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	cio::cinfo << __func__ << ": ok" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__small_order__even


void free_list_test__small_order__odd() {
	void *pages = memory::alloc_pages(128);

	if (!pages) {
		cio::cerr << __func__ << ": allocation failed!" << cio::endl;
		panic("FAILED TEST");
	}

	memory::page_frame_t *first_address =
		reinterpret_cast<memory::page_frame_t *>(
			memory::align_up<PAGE_SIZE*2>(pages)) + 1;
	memory::page_frame_t *second_address =
		first_address + 1;

	memory::page_frame_t *third_address =
		second_address + 1;

	memory::allocators::FreeList fl;
	if (!fl.initialize(0)) {
		cio::cerr << __func__ << ": initialization failed!" << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(first_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(second_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(third_address) != second_address) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.get() != first_address) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	cio::cinfo << __func__ << ": ok" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__small_order__odd


void free_list_test__high_order__even() {
	void *pages = memory::alloc_pages(128);

	if (!pages) {
		cio::cerr << __func__ << ": allocation failed!" << cio::endl;
		panic("FAILED TEST");
	}

	memory::page_frame_t *first_address =
		reinterpret_cast<memory::page_frame_t *>(
			memory::align_up<PAGE_SIZE*16>(pages));
	memory::page_frame_t *second_address =
		first_address + 8;

	memory::page_frame_t *third_address =
		second_address + 8;

	memory::allocators::FreeList fl;
	if (!fl.initialize(3, true)) {
		cio::cerr << __func__ << ": initialization failed!" << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(first_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(second_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(third_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	size_t got_addr = reinterpret_cast<size_t>(fl.get()) / PAGE_SIZE;
	if (got_addr != reinterpret_cast<size_t>(first_address) / PAGE_SIZE) {
		cio::cerr << __func__ << ": bug on (got_addr=" << got_addr
			<< ") " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	cio::cinfo << __func__ << ": ok" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__high_order__even


void free_list_test__high_order__odd() {
	void *pages = memory::alloc_pages(128);

	if (!pages) {
		cio::cerr << __func__ << ": allocation failed!" << cio::endl;
		panic("FAILED TEST");
	}

	memory::page_frame_t *first_address =
		reinterpret_cast<memory::page_frame_t *>(
			memory::align_up<PAGE_SIZE*16>(pages)) + 8;
	memory::page_frame_t *second_address =
		first_address + 8;

	memory::page_frame_t *third_address =
		second_address + 8;

	memory::allocators::FreeList fl;
	if (!fl.initialize(3, true)) {
		cio::cerr << __func__ << ": initialization failed!" << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(first_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(second_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.put(third_address) != nullptr) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	if (fl.get() != first_address) {
		cio::cerr << __func__ << ": bug on " << __LINE__ << cio::endl;
		panic("FAILED TEST");
	}

	cio::cinfo << __func__ << ": ok" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__high_order__odd


} // namespace


void ost::buddy_allocator_test() {
	constexpr size_t PAGES = 1023;
	memory::allocators::pblk_t blk;

	blk.size = PAGES;
	blk.ptr = reinterpret_cast<memory::page_frame_t *>(
			memory::alloc_pages(blk.size));

	if (!blk.ptr) {
		cio::cerr << __func__ << ": allocation failed!" << cio::endl;
		panic("FAILED TEST");
	}

	memory::allocators::BuddyAllocator<3> buddy_system;

	buddy_system.put(blk);

	memory::allocators::pblk_t pages[PAGES];
	for (size_t page_idx = 0; page_idx != PAGES; ++page_idx) {
		pages[page_idx] = buddy_system.get(1);
		if (pages[page_idx].ptr == nullptr) {
			cio::cerr << __func__ << ": failed ["
				<< page_idx << "] = " << pages[page_idx].ptr
				<< cio::endl;
			panic("FAILED TEST");
		}

	}

	auto last_alloc = buddy_system.get(1);
	if (last_alloc.ptr != nullptr) {
		cio::cerr << __func__ << ": failed [last] = "
			<< last_alloc.ptr << cio::endl;
		panic("FAILED TEST");
	}

	for (size_t page_idx = 0; page_idx != PAGES; ++page_idx) {
		buddy_system.put(pages[page_idx]);
	}

	for (size_t page_idx = 0; page_idx != PAGES; ++page_idx) {
		pages[page_idx] = buddy_system.get(1);
		if (pages[page_idx].ptr == nullptr) {
			cio::cerr << __func__ << ": failed ["
				<< page_idx << "] = " << pages[page_idx].ptr
				<< cio::endl;
			panic("FAILED TEST");
		}

	}

	last_alloc = buddy_system.get(1);
	if (last_alloc.ptr != nullptr) {
		cio::cerr << __func__ << ": failed [last] = "
			<< last_alloc.ptr << cio::endl;
		panic("FAILED TEST");
	}

	cio::cinfo << __func__ << ": ok" << cio::endl;

	memory::free_pages(blk.ptr);
} // buddy_allocator_test


void ost::free_list_test() {
	free_list_test__small_order__even();
	free_list_test__small_order__odd();
	free_list_test__high_order__even();
	free_list_test__high_order__odd();
}


namespace {

void try_alloc(size_t bytes) {
	auto mem = memory::kmalloc(bytes);
	if (!mem) {
		cio::ccrit << __func__ << ": failed allocation of size "
			<< bytes << "\n" << cio::endl;
		panic("FAILED TEST");
	}
	memory::kfree(mem);
}

}

void ost::mallocator_test() {
	for (size_t chunk_size = 7; chunk_size < PAGE_SIZE*3;
			chunk_size += 8) {
		cio::cinfo << __func__ << "[" << chunk_size << "]:";
		try_alloc(chunk_size);
		cio::cinfo << cio::endl;
	}
}
#else
void ost::page_alloc_test() {
}
void ost::slab_test() {
}
void ost::free_list_test() {
}
void ost::buddy_allocator_test() {
}
void ost::mallocator_test() {
}
#endif
