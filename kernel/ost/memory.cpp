#include "memory.hpp"

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/ost.hpp>
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

	for (size_t i = 0; i != 5; ++i)
		OST_ASSERT(p[i] == q[i], i, ": ", p[i], " vs ", q[i]);

	cio::cinfo << __func__ << ": OK" << cio::endl;
}

void ost::slab_test() {
	memory::allocators::SlabAllocator test_slab(sizeof(long), 10);
	OST_ASSERT(test_slab.is_initialized(), "slab initialization failure");

	long *p[5];
	p[0] = reinterpret_cast<long *>(test_slab.allocate());
	p[1] = reinterpret_cast<long *>(test_slab.allocate());
	p[2] = reinterpret_cast<long *>(test_slab.allocate());
	p[3] = reinterpret_cast<long *>(test_slab.allocate());
	test_slab.deallocate(p[2]);
	p[4] = reinterpret_cast<long *>(test_slab.allocate());

	OST_ASSERT(p[1] == p[0] + 1);
	OST_ASSERT(p[2] == p[1] + 1);
	OST_ASSERT(p[3] == p[2] + 1);
	OST_ASSERT(p[4] == p[2]);
}


namespace {

void free_list_test__small_order__even() {
	auto *pages = reinterpret_cast<memory::page_frame_t *>(
			memory::alloc_pages(128));
	OST_ASSERT(pages, "allocation failed");

	auto *first_address = memory::align_up<PAGE_SIZE*2>(pages);
	auto *second_address = first_address + 1;
	auto *third_address = second_address + 1;

	memory::allocators::FreeList fl;
	OST_ASSERT(fl.initialize(0),"initialization failed");

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == first_address);
	OST_ASSERT(fl.put(third_address) == nullptr);
	OST_ASSERT(fl.get() == third_address);
	OST_ASSERT(fl.get() == nullptr);

	cio::cinfo << __func__ << ": OK" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__small_order__even


void free_list_test__small_order__odd() {
	auto *pages = reinterpret_cast<memory::page_frame_t *>(memory::alloc_pages(128));
	OST_ASSERT(pages, "allocation failed");

	auto *first_address = memory::align_up<PAGE_SIZE*2>(pages) + 1;
	auto *second_address = first_address + 1;
	auto *third_address = second_address + 1;

	memory::allocators::FreeList fl;
	OST_ASSERT(fl.initialize(0), "initialization failed");

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == second_address);
	OST_ASSERT(fl.get() == first_address);

	cio::cinfo << __func__ << ": OK" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__small_order__odd


void free_list_test__high_order__even() {
	auto pages = reinterpret_cast<memory::page_frame_t *>(
			memory::alloc_pages(128));
	OST_ASSERT(pages, "allocation failed");

	auto first_address = memory::align_up<PAGE_SIZE*16>(pages);
	auto *second_address = first_address + 8;
	auto *third_address = second_address + 8;

	memory::allocators::FreeList fl;
	OST_ASSERT(fl.initialize(3, true), "initialization failed");

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == nullptr);
	OST_ASSERT(fl.get() == first_address);
	OST_ASSERT(fl.get() == second_address);
	OST_ASSERT(fl.get() == third_address);
	OST_ASSERT(fl.get() == nullptr);

	cio::cinfo << __func__ << ": OK" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__high_order__even


void free_list_test__high_order__odd() {
	auto pages = reinterpret_cast<memory::page_frame_t *>(
			memory::alloc_pages(128));
	OST_ASSERT(pages, "allocation failed");

	auto *first_address = memory::align_up<PAGE_SIZE*16>(pages) + 8;
	auto *second_address = first_address + 8;
	auto *third_address = second_address + 8;

	memory::allocators::FreeList fl;
	OST_ASSERT(fl.initialize(3, true), "initialization failed");

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == nullptr);
	OST_ASSERT(fl.get() == first_address);
	OST_ASSERT(fl.get() == second_address);
	OST_ASSERT(fl.get() == third_address);
	OST_ASSERT(fl.get() == nullptr);

	cio::cinfo << __func__ << ": ok" << cio::endl;

	memory::free_pages(pages);
} // free_list_test__high_order__odd


} // namespace


void ost::buddy_allocator_test() {
	constexpr size_t PAGES = 800;
	memory::allocators::pblk_t blk;

	blk.size = PAGES + 223;
	blk.ptr = reinterpret_cast<memory::page_frame_t *>(
			memory::alloc_pages(blk.size));

	OST_ASSERT(blk.ptr, "allocation failed");

	memory::MemoryRegion region;
	region.begin(blk.ptr);
	region.end(blk.ptr + blk.size);
	memory::allocators::BuddyAllocator buddy_system;

	buddy_system.initialize(&region, 3);
	buddy_system.put(blk);

	memory::allocators::pblk_t pages[PAGES];
	for (size_t page_idx = 0; page_idx != PAGES; ++page_idx) {
		pages[page_idx] = buddy_system.get(1);
		OST_ASSERT(pages[page_idx].ptr);
	}

	for (size_t page_idx = 0; page_idx != PAGES; ++page_idx) {
		buddy_system.put(pages[page_idx]);
	}

	for (size_t page_idx = 0; page_idx != PAGES; ++page_idx) {
		OST_ASSERT(pages[page_idx] == buddy_system.get(1));
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


void ost::mallocator_test() {
	for (size_t chunk_size = 7; chunk_size < PAGE_SIZE*3;
			chunk_size += 8) {
		auto mem = memory::kmalloc(chunk_size);
		OST_ASSERT(mem, "chunk_size = ", chunk_size);
		memory::kfree(mem);
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
