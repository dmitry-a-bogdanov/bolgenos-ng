#include <bolgenos-ng/memory.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/memory_region.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/page.hpp>

#include "buddy_allocator.hpp"
#include "page_allocator.hpp"

#include "config.h"

void memset(void *mem, char val, size_t size) {
	for(size_t pos = 0; pos != size; ++pos) {
		write_8(((char *)mem) + pos, &val);
	}
}


/**
* \brief Start of kernel object.
*
* Pointer to start of memory where kernel ELF is loaded.
*
*/
_asm_linked_ char __kernel_obj_start[0];


/**
* \brief End of kernel object.
*
* Pointer to end of memory where kernel ELF is loaded.
*/
_asm_linked_ char __kernel_obj_end[0];


namespace {


/// \brief Start of high memory.
///
/// The address of the beginning of high memory.
memory::page_frame_t * const highmem_start
	= reinterpret_cast<memory::page_frame_t *>(0x100000);


void detect_memory_regions();
void initilize_highmem_allocator();

} // namespace





namespace {


using memory::allocators::PageAllocator;
using memory::allocators::BuddyAllocator;
using memory::MemoryRegion;


/// \brief High memory.
///
/// High memory region descriptor
MemoryRegion highmem;


/// Buddy system that is built on the \ref highmem memory region.
BuddyAllocator<PageAllocator::buddy_order::value> highmem_buddy_allocator;


/// Page allocator that is built on the \ref highmem_buddy_allocator.
PageAllocator highmem_page_allocator;


} // namespace


void *memory::alloc_pages(size_t n) {
	return highmem_page_allocator.allocate(n);
}


void memory::free_pages(void *address) {
	highmem_page_allocator.deallocate(address);
}


void memory::init() {
	detect_memory_regions();
	initilize_highmem_allocator();
}

namespace {


void detect_memory_regions() {
	if (multiboot::boot_info->is_meminfo_valid()) {
		cio::cnotice << "Detected memory: "
			<< "low = "
			<< multiboot::boot_info->low_memory() << " kB, "
			<< "high = "
			<< multiboot::boot_info->high_memory() << " kB"
			<< cio::endl;
	} else {
		panic("Bootloader didn't provide memory info!\n");
	}

	auto highmem_bytes = multiboot::boot_info->high_memory() * 1024;

	highmem.begin(highmem_start);
	highmem.end(highmem_start + memory::align_down<PAGE_SIZE>(highmem_bytes) / PAGE_SIZE);
}


void initilize_highmem_allocator() {
	auto *last_kernel_page = reinterpret_cast<memory::page_frame_t *>(
			memory::align_up<PAGE_SIZE>(__kernel_obj_end));

	highmem_buddy_allocator.initialize(&highmem);
	highmem_page_allocator.initialize(&highmem_buddy_allocator,
			last_kernel_page);
}

} // namespace

void operator delete(void *) {
	// empty declaration
}

