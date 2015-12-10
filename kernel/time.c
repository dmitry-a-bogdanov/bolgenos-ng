#include <bolgenos-ng/time.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>

volatile uint32_t ticks = 0;

#if defined(CONFIG_VERBOSE_TIMER_INTERRUPT)
#define VERBOSE_TIMER_INTERRUPT 1
#else
#define VERBOSE_TIMER_INTERRUPT 0
#endif

#define __TIMER_IRQ	(min_pic_irq + 0)

static void handle_timer_interrupt(irq_t vector);

void init_timer() {
	register_irq_handler(__TIMER_IRQ, handle_timer_interrupt);
}

static void handle_timer_interrupt(irq_t vector __attribute__((unused))) {
	++ticks;
#if VERBOSE_TIMER_INTERRUPT
	printk("tick #%lu\n", ticks);
#endif
}

void __sleep(uint32_t ticks_timeout) {
	uint32_t end_of_sleep = ticks + ticks_timeout;
	while (ticks < end_of_sleep) {
		halt_cpu();
	}
}
