#include <bolgenos-ng/memory.h>

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
struct __attribute__((packed)) page {
	struct page *next; /*!< Next page in allocation block. */
	int free:1; /*!< Page is free flag. */
};


/**
* \brief Page with zero size.
*
* This address will be returned if allocator is called with zero size argument.
*/
#define ZERO_PAGE			((void *)0x10)


/**
*
* Used only for simplifying address arithmetics.
*/
struct __attribute__((packed)) page_frame {
	char __data[PAGE_SIZE];
};
assert_type_size(struct page_frame, PAGE_SIZE);


/**
* \brief Memory region descriptor.
*
* Structure describes memory regions like high and low memory.
*/
struct memory_region {
	size_t size;
	/*!< Number of allocatable pages in region. */

	struct page *pages;
	/*!< Pointer to array of page descriptors. */

	struct page_frame *frames;
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
	(((struct page *)(the_page)) - (m_region)->pages)


/**
* \brief Get page frame index.
*
* Get index of specified page frame in given memory region.
*
* \param m_region Pointer to memory region.
* \param frame Specified page frame.
*/
#define frame_index(m_region, frame) \
	(((struct page_frame *)(frame)) - (m_region)->frames)


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
	size_t required_memory = sizeof(struct page) * pages;
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
	for(struct page *name = (m_region)->pages;			\
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
	for(struct page *name = start;					\
		name != (m_region)->pages + (m_region)->size;		\
		++name)


void init_memory() {
	if (mboot_is_meminfo_valid()) {
		printk("Detected memory: low = %lu kb, high = %lu kb\n",
			(long unsigned) mboot_get_low_mem(),
			(long unsigned) mboot_get_high_mem());
	} else {
		panic("Bootloader didn't provide memory info!\n");
	}

	struct page_frame *highmem_first_free = (struct page_frame *)
		align_up((size_t) __kernel_obj_end, PAGE_SIZE);

	// points to page that contains last RAM address.
	struct page_frame *highmem_last_free = (struct page_frame *)
		align_down((size_t) ( __high_memory_start
			+ mboot_get_high_mem() * 1024),
			PAGE_SIZE);

	printk("[MEM_INFO] highmem free frames: %lu...%lu\n",
		(long unsigned) highmem_first_free,
		(long unsigned) highmem_last_free);

	size_t highmem_free_pages = highmem_last_free - highmem_first_free;
	printk("[MEM_INFO] highmem_free_pages=%lu\n", highmem_free_pages);

	mem_split_t highmem_split;
	split_pages(highmem_free_pages, &highmem_split);
	high_memory.size = highmem_split.frames;
	high_memory.pages = (struct page *) highmem_first_free;
	high_memory.frames = highmem_first_free + highmem_split.pages;

	for_each_page(&high_memory, p) {
		p->free = MEM_FREE;
		p->next = NULL;
	}

	printk("[MEM_INFO] high_memory: size=%lu, pages=%lu, frames=%lu\n",
		(unsigned long) high_memory.size,
		(unsigned long) high_memory.pages,
		(unsigned long) high_memory.frames);
}


/**
* \brief Mark pages as allocated.
*
* The function marks specified page range as allocated and connected pages
* to one page allocation block.
* \param from First page in block.
* \param n Number of pages in block.
*/
static void __alloc_pages(struct page *from, size_t n) {
	for (struct page *prev_page = NULL, *page = from; page != from + n;
			++page) {
		page->free = MEM_USED;
		page->next = NULL;
		if (prev_page) {
			prev_page->next = page;
		}
		prev_page = page;
	}
}


/**
* \brief Find free pages.
*
* The function finds specified number of continious free pages in the given
* memory region.
*
* \param region Given memory region.
* \param n Number of free pages to find.
* \return Pointer to first free page or NULL.
*/
static struct page *__find_free_pages(struct memory_region *region, size_t n) {
	struct page *page_block = NULL;
	for_each_page(region, page) {
		if (page->free == MEM_USED) {
			continue;
		}
		size_t free_pages = 0;
		for_each_page_from(region, other_page, page) {
			if (other_page->free == MEM_USED)
				break;
			++free_pages;
			if (free_pages == n)
				break;
		}

		if (free_pages == n) {
			page_block = page;
			break;
		}
	}
	return page_block;
}


void *alloc_pages(size_t n) {
	if (n == 0)
		return ZERO_PAGE;
	struct page_frame *first_frame = NULL;
	struct page *free_page_block = __find_free_pages(&high_memory, n);
	if (free_page_block) {
		__alloc_pages(free_page_block, n);
		first_frame = high_memory.frames + page_index(&high_memory,
				free_page_block);
	}
	return (void*) first_frame;
}


void free_pages(void *addr) {
	if (addr == NULL || addr == ZERO_PAGE)
		return;

	struct page *page = high_memory.pages + frame_index(&high_memory, addr);
	struct page *next;
	do {
		if (page->free) {
			panic("Double freeing was detected\n");
		}
		next = page->next;
		page->free = MEM_FREE;
		page->next = NULL;
		page = next;
	} while (next != NULL);
}
