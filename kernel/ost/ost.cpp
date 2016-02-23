#include <bolgenos-ng/ost.hpp>

#include <bolgenos-ng/printk.h>

#include <ost.h>
#include "memory.hpp"

#ifdef OST_ENABLE_TESTS
void ost::run() {
	printk("OST: running...\n");
	ost::memory_test();
	printk("OST: finished\n");
}
#else
void ost::run() {
	printk("OST is disabled\n");
}
#endif
