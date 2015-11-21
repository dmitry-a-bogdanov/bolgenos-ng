#include <bolgenos-ng/time.h>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>

volatile uint32_t ticks = 0;

#if defined(CONFIG_VERBOSE_TIMER_INTERRUPT)
#define VERBOSE_TIMER_INTERRUPT 1
#else
#define VERBOSE_TIMER_INTERRUPT 0
#endif

void handle_timer_interrupt() {
#if VERBOSE_TIMER_INTERRUPT
	++ticks;
	char info[30];
	snprintf(info, 30, "tick #%lu\n", ticks);
	vga_console_puts(info);
#else
	++ticks;
#endif
}
