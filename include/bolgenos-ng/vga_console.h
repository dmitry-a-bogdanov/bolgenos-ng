#ifndef __BOLGENOS_NG__VGA_CONSOLE_H__
#define __BOLGENOS_NG__VGA_CONSOLE_H__ (1)

typedef enum {
	vga_black			= 0,
	vga_blue			= 1,
	vga_green			= 2,
	vga_cyan			= 3,
	vga_red				= 4,
	vga_magenta			= 5,
	vga_brown			= 6,
	vga_grey			= 7,
	vga_dark_grey			= 8,
	vga_bright_blue			= 9,
	vga_bright_green		= 10,
	vga_bright_cyan			= 11,
	vga_bright_red			= 12,
	vga_bright_magenta		= 13,
	vga_yellow			= 14,
	vga_white			= 15
} vga_color_t;

void vga_console_init();

void vga_set_bg(vga_color_t bg);
vga_color_t vga_get_bg();

void vga_set_fg(vga_color_t fg);
vga_color_t vga_get_fg();

void vga_clear_screen();

void vga_console_putc(char c);
void vga_console_putc_color(char c, vga_color_t fg, vga_color_t bg);
void vga_console_puts(const char* str);
void vga_console_puts_color(const char* str, vga_color_t fg, vga_color_t bg);

#endif // __BOLGENOS_NG__VGA_CONSOLE_H__
