#include <bolgenos-ng/error.h>

#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/irq.hpp>

void panic(const char *msg) {
	irq::disable();
	printk("Kernel Panic:\n");
	printk(msg);
	while(1) {
		halt_cpu();
	}
}

void bug(const char *msg) {
	printk("Kernel Bug! Escalating to Kernel Panic...\n");
	panic(msg);
}
