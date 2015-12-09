#include <bolgenos-ng/error.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/printk.h>

void panic(const char *msg) {
	interrupts_disable();
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
