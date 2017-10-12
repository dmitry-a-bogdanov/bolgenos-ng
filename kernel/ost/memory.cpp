#include "memory.hpp"

#include <cstdint>
#include <ostream>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/error.h>

#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/slab.hpp>


#include "../free_list.hpp"
#include "../buddy_allocator.hpp"
#include "../mallocator.hpp"

using memory::PAGE_SIZE;

using namespace std;
using namespace lib;

void ost::page_alloc_test()
{
	DEBUG_FUNCTION();

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
		OST_ASSERT(p[i] == q[i]) << "i = " << i << ": " << p[i] << " vs " << q[i];
}

void ost::slab_test()
{
	DEBUG_FUNCTION();

	memory::allocators::SlabAllocator test_slab(sizeof(long), 10);
	OST_ASSERT(test_slab.is_initialized());

	long *p[5];
	p[0] = static_cast<long *>(test_slab.allocate());
	p[1] = static_cast<long *>(test_slab.allocate());
	p[2] = static_cast<long *>(test_slab.allocate());
	p[3] = static_cast<long *>(test_slab.allocate());
	test_slab.deallocate(p[2]);
	p[4] = static_cast<long *>(test_slab.allocate());

	OST_ASSERT(p[1] == p[0] + 1);
	OST_ASSERT(p[2] == p[1] + 1);
	OST_ASSERT(p[3] == p[2] + 1);
	OST_ASSERT(p[4] == p[2]);
}


namespace {

void free_list_test__small_order__even()
{
	DEBUG_FUNCTION();

	auto *pages = static_cast<memory::page_frame_t *>(
			memory::alloc_pages(128));
	OST_ASSERT(pages);

	auto *first_address = memory::align_up<PAGE_SIZE*2>(pages);
	auto *second_address = first_address + 1;
	auto *third_address = second_address + 1;

	memory::allocators::FreeList fl;

	OST_ASSERT(fl.initialize(0, false));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == first_address);
	OST_ASSERT(fl.put(third_address) == nullptr);
	OST_ASSERT(fl.get() == third_address);
	OST_ASSERT(fl.get() == nullptr);

	memory::free_pages(pages);
}


void free_list_test__small_order__odd()
{
	DEBUG_FUNCTION();

	auto *pages = static_cast<memory::page_frame_t *>(
		memory::alloc_pages(128));
	OST_ASSERT(pages);

	auto *first_address = memory::align_up<PAGE_SIZE*2>(pages) + 1;
	auto *second_address = first_address + 1;
	auto *third_address = second_address + 1;

	memory::allocators::FreeList fl;
	OST_ASSERT(fl.initialize(0));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == second_address);
	OST_ASSERT(fl.get() == first_address);

	memory::free_pages(pages);
}


void free_list_test__high_order__even()
{
	DEBUG_FUNCTION();

	auto pages = static_cast<memory::page_frame_t *>(
			memory::alloc_pages(128));
	OST_ASSERT(pages);

	auto first_address = memory::align_up<PAGE_SIZE*16>(pages);
	auto *second_address = first_address + 8;
	auto *third_address = second_address + 8;

	memory::allocators::FreeList fl;
	OST_ASSERT(fl.initialize(3, true));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == nullptr);

	auto got_addr = reinterpret_cast<uintptr_t>(fl.get()) / PAGE_SIZE;
	OST_ASSERT(got_addr
		== reinterpret_cast<std::uintptr_t>(first_address) / PAGE_SIZE);

	memory::free_pages(pages);
}


void free_list_test__high_order__odd()
{
	DEBUG_FUNCTION();

	auto pages = static_cast<memory::page_frame_t *>(
			memory::alloc_pages(128));

	OST_ASSERT(pages);

	auto *first_address = memory::align_up<PAGE_SIZE*16>(pages) + 8;
	auto *second_address = first_address + 8;
	auto *third_address = second_address + 8;

	memory::allocators::FreeList fl;
	OST_ASSERT(fl.initialize(3, true));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == nullptr);
	OST_ASSERT(fl.get() == first_address);
	OST_ASSERT(fl.get() == second_address);
	OST_ASSERT(fl.get() == third_address);
	OST_ASSERT(fl.get() == nullptr);

	memory::free_pages(pages);
}


} // namespace



void ost::buddy_allocator_test()
{
	constexpr size_t PAGES = 800;


	memory::allocators::pblk_t blk;

	blk.size = PAGES;

	blk.ptr = static_cast<memory::page_frame_t *>(
			memory::alloc_pages(blk.size));

	OST_ASSERT(blk.ptr);


	memory::MemoryRegion region;
	region.begin(blk.ptr);
	region.end(blk.ptr + blk.size);
	memory::allocators::BuddyAllocator buddy_system;

	buddy_system.initialize(&region);
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

	memory::free_pages(blk.ptr);
}


void ost::free_list_test() {
	free_list_test__small_order__even();
	free_list_test__small_order__odd();
	free_list_test__high_order__even();
	free_list_test__high_order__odd();
}


void ost::mallocator_test()
{
	for (size_t chunk_size = 7; chunk_size < PAGE_SIZE*3;
			chunk_size += 8) {
		auto mem = memory::kmalloc(chunk_size);
		OST_ASSERT(mem) << "chunk_size = " << chunk_size;
		memory::kfree(mem);
	}
}



