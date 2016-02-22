#include <bolgenos-ng/memory.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/multiboot_info.h>
#include <bolgenos-ng/printk.h>

#include "config.h"


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


/**
* \brief Page descriptor structure.
*
* The structure is intended to describe page frame.
*/
struct __attribute__((packed)) page_t {
	page_t *next; /*!< Next page in allocation block. */
	bool free; /*!< Page is free flag. */
};


/**
* \brief Page with zero size.
*
* This address will be returned if allocator is called with zero size argument.
*/
#define ZERO_PAGE			((void *)0x10)


/**
* \brief Page in real memory.
*
* Used only for simplifying address arithmetics.
*/
struct __attribute__((packed)) page_frame_t {
	char __data[PAGE_SIZE];
};
static_assert(sizeof(page_frame_t) == PAGE_SIZE, "Wrong size of page_frame_t");


/**
* \brief Memory region descriptor.
*
* Structure describes memory regions like high and low memory.
*/
struct memory_region {
	size_t size;
	/*!< Number of allocatable pages in region. */

	page_t *pages;
	/*!< Pointer to array of page descriptors. */

	page_frame_t *frames;
	/*!< Pointer to region of pages frames. */
};


/**
* \brief Get page index.
*
* Get index of specified page in given memory region.
*
* \param m_region Pointer to memory region.
* \param the_page Specified page.
*/
#define page_index(m_region, the_page) \
	(((page_t *)(the_page)) - (m_region)->pages)


/**
* \brief Get page frame index.
*
* Get index of specified page frame in given memory region.
*
* \param m_region Pointer to memory region.
* \param frame Specified page frame.
*/
#define frame_index(m_region, frame) \
	(((page_frame_t *)(frame)) - (m_region)->frames)


/**
* \brief Calculate number of pages for page descriptors.
*
* The function calculates number of pages that is needed to keep page
* descriptors for specified number of pages.
*
* \param pages Number of pages.
* \return Number of pages for holding page descriptors.
*/
static size_t descriptor_pages(size_t pages) {
	size_t required_memory = sizeof(page_t) * pages;
	return align_up(required_memory, PAGE_SIZE) / PAGE_SIZE;
}


/**
* \brief Split page result.
*
* Structure for returning result of splitting memory region.
*/
typedef struct {
	size_t pages; /*!< Number of pages for page descriptors. */
	size_t frames; /*!< Number of allocatable frames. */
} mem_split_t;


/**
* \brief Split pages in memory region into descriptors and frames.
*
* Function splits pages from memory region into two sections: page descriptors
* area and page frames area.
*
* \param free_pages Number of free pages in area.
* \param split Pointer to \ref mem_split_t structure for keeping result.
*/
static void split_pages(size_t free_pages, mem_split_t *split) {
	size_t pages = free_pages;
	while ( descriptor_pages(pages) + pages != free_pages && pages != 0)
		--pages;
	split->frames = pages;
	split->pages = free_pages - pages;
}


/**
* \brief High memory.
*
* High memory region descriptor.
*/
static struct memory_region high_memory;


/**
* \brief Start of high memory.
*
* The address of the beginning of high memory.
*/
#define __high_memory_start		((char *) 0x100000)


/**
* \brief For each page loop.
*
* Loop through all page descriptor in memory region.
*
* \param m_region Pointer to memory region for looping.
* \param name Name of iterator variable to be created.
*/
#define for_each_page(m_region, name)					\
	for(page_t *name = (m_region)->pages;			\
		name != (m_region)->pages + (m_region)->size;		\
		++name)


/**
* \brief For each page loop starting from specified page.
*
* Loop through all page descriptor in memory region starting from specified
* page.
*
* \param m_region Pointer to memory region for looping.
* \param name Name of iterator variable to be created.
* \param start Pointer to the first page descriptor for looping.
*/
#define for_each_page_from(m_region, name, start)			\
	for(page_t *name = start;					\
		name != (m_region)->pages + (m_region)->size;		\
		++name)


#define RUN_MEMORY_TEST
#if defined(RUN_MEMORY_TEST)
void run_memory_test();
#endif

void init_memory() {
	if (mboot_is_meminfo_valid()) {
		printk("Detected memory: low = %lu kb, high = %lu kb\n",
			(long unsigned) mboot_get_low_mem(),
			(long unsigned) mboot_get_high_mem());
	} else {
		panic("Bootloader didn't provide memory info!\n");
	}

	page_frame_t *highmem_first_free = reinterpret_cast<page_frame_t *>(
		align_up((size_t) __kernel_obj_end, PAGE_SIZE));

	// points to page that contains last RAM address.
	page_frame_t *highmem_last_free = reinterpret_cast<page_frame_t *>(
		align_down((size_t) ( __high_memory_start
			+ mboot_get_high_mem() * 1024),
			PAGE_SIZE));

	printk("[MEM_INFO] highmem free frames: %lu...%lu\n",
		(long unsigned) highmem_first_free,
		(long unsigned) highmem_last_free);

	size_t highmem_free_pages = highmem_last_free - highmem_first_free;
	printk("[MEM_INFO] highmem_free_pages=%lu\n", highmem_free_pages);

	mem_split_t highmem_split;
	split_pages(highmem_free_pages, &highmem_split);
	high_memory.size = highmem_split.frames;
	high_memory.pages = (page_t *) highmem_first_free;
	high_memory.frames = highmem_first_free + highmem_split.pages;

	for_each_page(&high_memory, p) {
		p->free = true;
		p->next = nullptr;
	}

	printk("[MEM_INFO] high_memory: size=%lu, pages=%lu, frames=%lu\n",
		(unsigned long) high_memory.size,
		(unsigned long) high_memory.pages,
		(unsigned long) high_memory.frames);

#ifdef RUN_MEMORY_TEST
	run_memory_test();
#endif
}


/**
* \brief Mark pages as allocated.
*
* The function marks specified page range as allocated and connected pages
* to one page allocation block.
* \param from First page in block.
* \param n Number of pages in block.
*/
static void __alloc_pages(page_t *from, size_t n) {
	for (page_t *prev_page = nullptr, *page = from; page != from + n;
			++page) {
		page->free = false;
		page->next = nullptr;
		if (prev_page) {
			prev_page->next = page;
		}
		prev_page = page;
	}
}


size_t has_page_block(memory_region *region, page_t *page, size_t n) {
	page_t *last = region->pages + region->size;
	size_t cont_free_pages = 0;
	for (; page != last && cont_free_pages != n; ++page) {
		if (!page->free)
			break;
		++cont_free_pages;
	}
	return cont_free_pages;
}

/**
* \brief Find free pages.
*
* The function finds specified number of continious free pages in the given
* memory region.
*
* \param region Given memory region.
* \param n Number of free pages to find.
* \return Pointer to first free page or nullptr.
*/
static page_t *__find_free_pages(memory_region *region, size_t n) {
	page_t *page_block = nullptr;
	for_each_page(region, page_iterator) {
		if (!page_iterator->free) {
			continue;
		}
		size_t free_pages = has_page_block(region, page_iterator, n);
		if (free_pages == n) {
			page_block = page_iterator;
			break;
		} else {
			page_iterator += free_pages;
		}
	}
	return page_block;
}


void *alloc_pages(size_t n) {
	if (n == 0)
		return ZERO_PAGE;
	page_frame_t *first_frame = nullptr;
	page_t *free_page_block = __find_free_pages(&high_memory, n);
	if (free_page_block) {
		__alloc_pages(free_page_block, n);
		first_frame = high_memory.frames + page_index(&high_memory,
				free_page_block);
	}
	return (void*) first_frame;
}


void free_pages(void *addr) {
	if (addr == nullptr || addr == ZERO_PAGE)
		return;

	page_t *page = high_memory.pages + frame_index(&high_memory, addr);
	page_t *next;
	do {
		if (page->free) {
			panic("Double freeing was detected\n");
		}
		next = page->next;
		page->free = true;
		page->next = nullptr;
		page = next;
	} while (next != nullptr);
}


void run_memory_test() {
	char *allocated[5];
	allocated[0] = reinterpret_cast<char*>(alloc_pages(1));
	allocated[1] = reinterpret_cast<char*>(alloc_pages(2));
	allocated[2] = reinterpret_cast<char*>(alloc_pages(3));
	allocated[3] = reinterpret_cast<char*>(alloc_pages(2));
	free_pages(allocated[1]);
	allocated[4] = reinterpret_cast<char*>(alloc_pages(2));

	if (allocated[1] == allocated[0] + PAGE_SIZE &&
		allocated[2] == allocated[1] + PAGE_SIZE*2 &&
		allocated[3] == allocated[2] + PAGE_SIZE*3 &&
		allocated[4] == allocated[1]) {
		printk("%s: ok\n", __func__);
	} else {
		printk("%s: fail: ");
		for (int i = 0; i < 5; ++i) {
			printk("a[%lu] = %lu ", (long unsigned) i,
				(long unsigned) allocated[i]);
		}
		printk("\n");
		panic("");
	}
}
