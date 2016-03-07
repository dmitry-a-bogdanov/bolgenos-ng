#include <bolgenos-ng/memory.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/memory_region.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/page.hpp>

#include "buddy_allocator.hpp"

#include "config.h"

void memset(void *mem, char val, size_t size) {
	for(size_t pos = 0; pos != size; ++pos) {
		write_8(((char *)mem) + pos, &val);
	}
}


memory::MemoryRegion memory::highmem;


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


/// \brief Page with zero size.
///
/// This address will be returned if allocator is called with
/// zero size argument.
//memory::page_frame_t *ZERO_PAGE =
//	reinterpret_cast<memory::page_frame_t *>(0x10);


/// \brief Start of high memory.
///
/// The address of the beginning of high memory.
memory::page_frame_t * const highmem_start
	= reinterpret_cast<memory::page_frame_t *>(0x100000);


memory::MemoryRegion highmem;


void detect_memory_regions();
void initilize_highmem_allocator();

} // namespace





namespace {


class StupidPageAllocator {
public:
	StupidPageAllocator() = default;
	StupidPageAllocator(const StupidPageAllocator&) = delete;
	StupidPageAllocator& operator =(const StupidPageAllocator&) = delete;


	bool initialize(memory::MemoryRegion *region, void *first_free);


	void *allocate(size_t n);


	void deallocate(void *address);


	void mark_as_already_in_use(memory::page_frame_t *frame);

protected:

	memory::MemoryRegion *region_ = nullptr;


	memory::page_t *pages_ = nullptr;
};


StupidPageAllocator high_memory_allocator;


} // namespace


bool StupidPageAllocator::initialize(memory::MemoryRegion *region,
		void *first_free) {

	region_ = region;

	// The whole page should be marked in-use.
	memory::page_frame_t *first_free_frame
		= reinterpret_cast<memory::page_frame_t *>(
			memory::align_up<PAGE_SIZE>(first_free));

	if (!region_->owns(first_free_frame)) {
		// all pages are in-use and no pages can be used for
		// keeping allocation map.
		panic("Bad first free\n");
		return false;
	}

	// The next page after last in-use page will be used for storing
	// allocation map.
	pages_ = reinterpret_cast<memory::page_t *>(first_free);


	// Mark all pages as free.
	for (memory::page_frame_t *frame = region_->begin();
			frame != region_->end(); ++frame) {
		size_t page_idx = region_->index_of(frame);
		pages_[page_idx].next = nullptr;
		pages_[page_idx].free = true;
	}

	// Mark aux pages (that are used for allocation map) as used.
	size_t aux_bytes = sizeof(memory::page_t) * region_->size();
	size_t aux_pages = memory::align_up<PAGE_SIZE>(aux_bytes) / PAGE_SIZE;

	for(memory::page_frame_t *frame = first_free_frame;
			frame != first_free_frame + aux_pages; ++frame) {
		mark_as_already_in_use(frame);
	}

	return false;
}

void StupidPageAllocator::mark_as_already_in_use(memory::page_frame_t *frame) {
	size_t page_idx = region_->index_of(frame);
	pages_[page_idx].next = nullptr;
	pages_[page_idx].free = false;
}


void *StupidPageAllocator::allocate(size_t n) {
	memory::page_frame_t *mem = nullptr;
	for(memory::page_t *pg = pages_; pg != pages_ + region_->size(); ++pg) {
		if (!pg->free) {
			continue;
		}
		size_t cont_free = 0;
		for(memory::page_t *next = pg;
				next != pages_ + region_->size(); ++next) {
			if (next->free) {
				++cont_free;
				if (cont_free == n)
					break;
			} else {
				break;
			}
		}
		if (cont_free == n) {
			mem = region_->begin() + (pg - pages_);
			break;
		}
	}
	if (mem) {
		auto mem_idx = region_->index_of(mem);
		for(size_t i = 0; i != n; ++i) {
			pages_[mem_idx + i].free = false;
			pages_[mem_idx + i].next = &pages_[mem_idx + i + 1];
		}
		pages_[mem_idx + n - 1].next = nullptr;
	}
	return reinterpret_cast<void *>(mem);
}


void StupidPageAllocator::deallocate(void *address) {
	auto frame = reinterpret_cast<memory::page_frame_t *>(address);
	auto page = &pages_[region_->index_of(frame)];
	do {
		page->free = true;
	} while(page++->next != nullptr);
}


void *memory::alloc_pages(size_t n) {
	return high_memory_allocator.allocate(n);
}

void memory::free_pages(void *address) {
	high_memory_allocator.deallocate(address);
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

	high_memory_allocator.initialize(&highmem,
			reinterpret_cast<void *>(__kernel_obj_end));
	memory::page_frame_t *first_kernel_page
		= reinterpret_cast<memory::page_frame_t *>(
			memory::align_down<PAGE_SIZE>(__kernel_obj_start));
	auto *last_kernel_page = reinterpret_cast<memory::page_frame_t *>(
			memory::align_up<PAGE_SIZE>(__kernel_obj_end));

	for(memory::page_frame_t *kernel_page = first_kernel_page;
			kernel_page != last_kernel_page; ++kernel_page) {
		high_memory_allocator.mark_as_already_in_use(kernel_page);
	}
}

} // namespace

void operator delete(void *) {
	// empty declaration
}

