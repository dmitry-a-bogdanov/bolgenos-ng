#include <cstdint>
#include <ostream>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/error.h>

#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/slab.hpp>


#include "../free_list.hpp"
#include "../buddy_allocator.hpp"
#include "../mallocator.hpp"

using namespace std;
//using namespace lib;
using namespace bolgenos;
using namespace memory;

TEST(PageAlloc, basic)
{
	void *p[5];
	p[0] = alloc_pages(1);
	p[1] = alloc_pages(2);
	p[2] = alloc_pages(3);
	p[3] = alloc_pages(2);
	free_pages(p[1]);
	p[4] = alloc_pages(2);
	free_pages(p[0]);
	free_pages(p[2]);
	free_pages(p[3]);
	free_pages(p[4]);

	void *q[5];
	q[0] = alloc_pages(1);
	q[1] = alloc_pages(2);
	q[2] = alloc_pages(3);
	q[3] = alloc_pages(2);
	free_pages(q[1]);
	q[4] = alloc_pages(2);

	for (size_t i = 0; i != 5; ++i)
		OST_ASSERT(p[i] == q[i]) << "i = " << i << ": " << p[i] << " vs " << q[i];
}

TEST(Slab, basic)
{
	allocators::SlabAllocator test_slab(sizeof(long), 10);
	OST_ASSERT(test_slab.is_initialized());

	long *p[5];
	p[0] = static_cast<long *>(test_slab.allocate());
	p[1] = static_cast<long *>(test_slab.allocate());
	p[2] = static_cast<long *>(test_slab.allocate());
	p[3] = static_cast<long *>(test_slab.allocate());
	test_slab.deallocate(p[2]);
	p[4] = static_cast<long *>(test_slab.allocate());

	OST_ASSERT_EQ(p[1], p[0] + 1);
	OST_ASSERT_EQ(p[2], p[1] + 1);
	OST_ASSERT_EQ(p[3], p[2] + 1);
	OST_ASSERT_EQ(p[4], p[2]);
}


TEST(FreeList, SmallOrderEven)
{
	auto *pages = static_cast<page_frame_t *>(
			alloc_pages(128));
	OST_ASSERT(pages);

	auto *first_address = align_up<PAGE_SIZE*2>(pages);
	auto *second_address = first_address + 1;
	auto *third_address = second_address + 1;

	allocators::FreeList fl;

	OST_ASSERT(fl.initialize(0, false));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == first_address);
	OST_ASSERT(fl.put(third_address) == nullptr);
	OST_ASSERT(fl.get() == third_address);
	OST_ASSERT(fl.get() == nullptr);

	free_pages(pages);
}


TEST(FreeList, SmallOrderOdd)
{
	auto *pages = static_cast<page_frame_t *>(
		alloc_pages(128));
	OST_ASSERT(pages);

	auto *first_address = align_up<PAGE_SIZE*2>(pages) + 1;
	auto *second_address = first_address + 1;
	auto *third_address = second_address + 1;

	allocators::FreeList fl;
	OST_ASSERT(fl.initialize(0));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == second_address);
	OST_ASSERT(fl.get() == first_address);

	free_pages(pages);
}

TEST(FreeList, HighOrderEven)
{
	auto pages = static_cast<page_frame_t *>(
			alloc_pages(128));
	OST_ASSERT(pages);

	auto first_address = align_up<PAGE_SIZE*16>(pages);
	auto *second_address = first_address + 8;
	auto *third_address = second_address + 8;

	allocators::FreeList fl;
	OST_ASSERT(fl.initialize(3, true));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == nullptr);

	auto got_addr = reinterpret_cast<uintptr_t>(fl.get()) / PAGE_SIZE;
	OST_ASSERT(got_addr
		== reinterpret_cast<std::uintptr_t>(first_address) / PAGE_SIZE);

	free_pages(pages);
}


TEST(FreeList, HighOrderOdd)
{
	auto pages = static_cast<page_frame_t *>(
			alloc_pages(128));

	OST_ASSERT(pages);

	auto *first_address = align_up<PAGE_SIZE*16>(pages) + 8;
	auto *second_address = first_address + 8;
	auto *third_address = second_address + 8;

	allocators::FreeList fl;
	OST_ASSERT(fl.initialize(3, true));

	OST_ASSERT(fl.put(first_address) == nullptr);
	OST_ASSERT(fl.put(second_address) == nullptr);
	OST_ASSERT(fl.put(third_address) == nullptr);
	OST_ASSERT(fl.get() == first_address);
	OST_ASSERT(fl.get() == second_address);
	OST_ASSERT(fl.get() == third_address);
	OST_ASSERT(fl.get() == nullptr);

	free_pages(pages);
}


TEST(BuddyAllocator, basic)
{
	constexpr size_t PAGES = 800;

	allocators::pblk_t blk;

	blk.size = PAGES;

	blk.ptr = static_cast<page_frame_t *>(
			alloc_pages(blk.size));

	OST_ASSERT(blk.ptr);


	MemoryRegion region;
	region.begin(blk.ptr);
	region.end(blk.ptr + blk.size);
	allocators::BuddyAllocator buddy_system;

	buddy_system.initialize(&region);
	buddy_system.put(blk);

	allocators::pblk_t pages[PAGES];
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

	free_pages(blk.ptr);
}

TEST(Mallocator, basic)
{
	for (size_t chunk_size = 7; chunk_size < PAGE_SIZE*3;
			chunk_size += 8) {
		auto mem = kmalloc(chunk_size);
		OST_ASSERT(mem) << "chunk_size = " << chunk_size;
		kfree(mem);
	}
}



