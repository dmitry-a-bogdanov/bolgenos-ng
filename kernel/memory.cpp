#include <bolgenos-ng/memory.hpp>


#include <bolgenos-ng/error.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/kernel_object.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/memory_region.hpp>
#include <bolgenos-ng/page.hpp>

#include "buddy_allocator.hpp"
#include "page_allocator.hpp"
#include "mallocator.hpp"
#include "multiboot_info.hpp"

using namespace memory;
using namespace memory::allocators;

void *operator new(std::size_t, void *address)
{
	return address;
}

static constexpr std::uintptr_t np_highmem_start = 0x100000;
page_frame_t * const highmem_start = reinterpret_cast<page_frame_t *>(np_highmem_start);

static void detect_memory_regions(const bolgenos::MultibootInfo*);
static void initilize_highmem_allocators();

static bool is_initialized_flag{ false };

static MemoryRegion highmem;
static BuddyAllocator highmem_buddy_allocator;
static PageAllocator highmem_page_allocator;
static Mallocator highmem_mallocator;


namespace bolgenos
{

void *alloc_pages(size_t n) {
	return highmem_page_allocator.allocate(n);
}


void free_pages(void *address) {
	highmem_page_allocator.deallocate(address);
}


void init_memory(const bolgenos::MultibootInfo* mboot_info) {
	detect_memory_regions(mboot_info);
	initilize_highmem_allocators();
	is_initialized_flag = true;
}


bool is_initialized()
{
    return is_initialized_flag;
}


void *kmalloc(size_t bytes) {
    if (!is_initialized())
    {
        panic("Memory was not initialized yet!");
    }
	return highmem_mallocator.allocate(bytes);
}

void kfree(void *memory) {
	return highmem_mallocator.deallocate(memory);
}


}

static MemoryRegion getHighmem(const bolgenos::MultibootInfo* mboot_info)
{
	MemoryRegion region;
	region.begin(highmem_start);
	auto highmem_bytes = mboot_info->high_memory() * 1024;
	region.end(highmem_start + bolgenos::align_down<PAGE_SIZE>(highmem_bytes) / PAGE_SIZE);
	return region;
}


static void detect_memory_regions(const bolgenos::MultibootInfo* mboot_info)
{
	if (!mboot_info->is_meminfo_valid())
	{
		panic("Bootloader didn't provide memory info!\n");
	}
	highmem = getHighmem(mboot_info);
	LOG_NOTICE("High memory: " << highmem.size() << " pages");
}


static void initilize_highmem_allocators() {
	auto *last_kernel_page = reinterpret_cast<page_frame_t *>(
			bolgenos::align_up<PAGE_SIZE>(kobj::end()));

	highmem_buddy_allocator.initialize(&highmem);
	highmem_page_allocator.initialize(&highmem_buddy_allocator,
			last_kernel_page);
	constexpr size_t chain_memory = 1024*1024; // 1 MB
	constexpr size_t chains = 33; // 32th step is 512
	highmem_mallocator.initialize(&highmem_page_allocator, chain_memory,
			chains);
}


