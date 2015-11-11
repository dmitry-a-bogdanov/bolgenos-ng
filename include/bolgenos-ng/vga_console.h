#ifndef __BOLGENOS_NG__VGA_CONSOLE_H__
#define __BOLGENOS_NG__VGA_CONSOLE_H__ (1)

void vga_console_init();

void vga_clear_screen();

void vga_console_putc(char);
void vga_console_puts(const char*);

#endif // __BOLGENOS_NG__VGA_CONSOLE_H__
