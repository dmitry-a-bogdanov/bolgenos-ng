#include <bolgenos-ng/memory.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/multiboot_info.h>
#include <bolgenos-ng/printk.h>


/**
* \brief Page size
* TODO: doc
*/
#define PAGE_SIZE			(4 * 1024)

/**
* \brief kernel object addressed.
* TODO: doc
*/
__link_from_asm__ char __kernel_obj_start[0];
__link_from_asm__ char __kernel_obj_end[0];

/**
* \brief Aling specified address down.
* TODO: doc
*/
static char *align_addr_down(char *addr, size_t boundary) {
	return (char *)(((size_t) addr) & ~(boundary - 1));
}


/**
* \brief Aling specified address up.
* TODO: doc
*/
static char *align_addr(char *addr, size_t boundary) {
	return (((size_t) addr) & (boundary - 1)) ?
		align_addr_down(addr, boundary) + boundary :
		addr;
}


enum pp_free {
	ppf_in_use		= 0x0,
	ppf_free		= 0x1,
};

typedef struct __attribute__((packed)) {
	enum pp_free		free:1;
} pp_descr_t;


/**
*
* Used only for simplifying address arithmetics.
*/
typedef struct __attribute__((packed)) {
	char __data[PAGE_SIZE];
} pp_frame_t;

assert_type_size(pp_frame_t, PAGE_SIZE);

struct memory_region {
	size_t size;
	/*!< Size of region in PAGE_SIZE units. */

	pp_descr_t *descriptors;
	/*!< Pointer to array of pseudo-page descriptors. */

	pp_frame_t *pages;
	/*!< Pointer to region of pseudo-pages. */
};


static size_t __descr_pages(size_t pages_number) {
	return (size_t) align_addr(
		(char *) (sizeof(pp_descr_t) * pages_number),
		PAGE_SIZE) / PAGE_SIZE;
}


static size_t __pages_available(size_t free_pages) {
	size_t pages = free_pages;
	while ( __descr_pages(pages) + pages != free_pages
			&& pages != 0)
		--pages;
	return pages;

}


static struct memory_region __system_memory;

static void dump_system_memory() {
	printk("[MEM_INFO] free memory: size = %lu, descriptors = %lu,"
		" pages = %lu\n", __system_memory.size,
		__system_memory.descriptors, __system_memory.pages);
}

/**
* Memory map: [descripor pages][memory pages]
*/

void init_memory() {
	if (mboot_is_meminfo_valid()) {
		printk("Detected memory: low = %lu kb, high = %lu kb\n",
			(long unsigned) mboot_get_low_mem(),
			(long unsigned) mboot_get_high_mem());
	} else {
		panic("Bootloader didn't provide memory info!\n");
	}

	pp_frame_t *kpages_last_addr =
		(pp_frame_t *) align_addr_down(__kernel_obj_end, PAGE_SIZE);

	size_t kpages_last_page = ((size_t) kpages_last_addr) / PAGE_SIZE;
	size_t free_page_first = kpages_last_page + 1;

	printk("[MEM_INFO] last kpage = %lu\n",
		(long unsigned) kpages_last_page);

	size_t high_mem_last_addr = (size_t) align_addr_down(
		(char *) (0x100000 + mboot_get_high_mem() * 1024),
		PAGE_SIZE);

	size_t high_mem_last_page = high_mem_last_addr/PAGE_SIZE;
	size_t free_page_last = high_mem_last_page - 1;

	printk("[MEM_INFO] free pages: %lu...%lu\n",
		(long unsigned) free_page_first,
		(long unsigned) free_page_last);

	__system_memory.size =
		__pages_available(free_page_last - free_page_first + 1);
	__system_memory.descriptors = (pp_descr_t *)
		(free_page_first * PAGE_SIZE);
	__system_memory.pages = ((pp_frame_t *) (free_page_first * PAGE_SIZE))
		+ __descr_pages(__system_memory.size);

	for(size_t p_i = 0; p_i != __system_memory.size; ++p_i) {
		__system_memory.descriptors[p_i].free = ppf_free;
	}

	dump_system_memory();
}

#define pp_descr_for_each(m_region, name)				\
	for(pp_descr_t *name = (m_region)->descriptors;			\
		name != (m_region)->descriptors + (m_region)->size;	\
		++name)

#define pp_descr_for_each_from(m_region, name, start)			\
	for(pp_descr_t *name = start;					\
		name != (m_region)->descriptors + (m_region)->size;	\
		++name)

void *alloc_pages(size_t n) {
	pp_frame_t *mem = NULL;
	pp_descr_for_each(&__system_memory, page) {
		if (page->free == ppf_in_use) {
			continue;
		}
		size_t free_pages = 0;
		pp_descr_for_each_from(&__system_memory, other_page, page) {
			if (other_page->free == ppf_in_use)
				break;
			++free_pages;
			if (free_pages == n)
				break;
		}

		size_t free_page_index = page - __system_memory.descriptors;

		if (free_pages == n) {
			mem = &__system_memory.pages[free_page_index];
			for (size_t i = 0; i != n; ++i) {
				__system_memory.descriptors[free_page_index
					+ i].free = ppf_in_use;
			}
			break;
		}
	}
	return mem;
}
