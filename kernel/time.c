#include <bolgenos-ng/time.h>

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
	char info[20];
	char *pptr = info;
	pptr += _strcpy(pptr, "tick #");
	pptr += uint32_to_string(ticks, pptr, 10);
	*pptr = '\n';
	++pptr;
	*pptr = '\0';
	vga_console_puts(info);
	++ticks;
#else
	++ticks;
#endif
}
