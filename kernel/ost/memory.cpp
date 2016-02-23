#include "memory.hpp"

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/memory.hpp>

#include <config.h>
#include <ost.h>

#ifdef OST_MEMORY
void ost::memory_test() {
	char *allocated[5];
	allocated[0] = reinterpret_cast<char*>(memory::alloc_pages(1));
	allocated[1] = reinterpret_cast<char*>(memory::alloc_pages(2));
	allocated[2] = reinterpret_cast<char*>(memory::alloc_pages(3));
	allocated[3] = reinterpret_cast<char*>(memory::alloc_pages(2));
	memory::free_pages(allocated[1]);
	allocated[4] = reinterpret_cast<char*>(memory::alloc_pages(2));

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
#else
void ost::memory_test() {
	// skiping memory test due to build configuration
}
#endif
