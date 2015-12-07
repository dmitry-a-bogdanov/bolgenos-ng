#include <bolgenos-ng/vga_console.h>

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>

static int __vga_height, __vga_width;
static int __vga_pos_line = 0, __vga_pos_column = 0;

#define __vga_offset (__vga_pos_line*__vga_width + __vga_pos_column)
#define __vga_pos (__vga_memory + __vga_offset)

#define __VGA_NEWLINE_SYMBOL '\n'
#define __VGA_CARRIAGE_RETURN_SYMBOL '\r'


typedef struct __attribute__((packed)) {
	char ch:8;
	vga_color_t fg:4;
	vga_color_t bg:4;
} vga_cell_t;

static vga_cell_t *__vga_memory;

static vga_color_t __current_bg;
static vga_color_t __current_fg;

void vga_set_bg(vga_color_t bg) {
	__current_bg = bg;
}

vga_color_t vga_get_bg() {
	return __current_bg;
}

void vga_set_fg(vga_color_t fg) {
	__current_fg = fg;
}

vga_color_t vga_get_fg() {
	return __current_fg;
}


static vga_cell_t __vga_cell(char symbol, vga_color_t fg, vga_color_t bg);
static void __vga_cursor_next();
static void __vga_linebreak();
static void __vga_newline();
static void __vga_scroll();
static void __vga_carriage_return();

void vga_console_putc_color(char symbol, vga_color_t fg,
		vga_color_t bg) {
	switch (symbol) {
	case __VGA_NEWLINE_SYMBOL:
		__vga_linebreak();
		break;
	case __VGA_CARRIAGE_RETURN_SYMBOL:
		__vga_carriage_return();
		break;
	default:
		{
			vga_cell_t cell = __vga_cell(symbol, fg, bg);
			write_16((char *)__vga_pos, address_of(cell));
			*__vga_pos = cell;
			__vga_cursor_next();
		}
	}
}

void vga_console_putc(char symbol) {
	vga_console_putc_color(symbol, __current_fg, __current_bg);
}

void vga_console_puts(const char* string) {
	vga_console_puts_color(string, __current_fg, __current_bg);
}

void vga_console_puts_color(const char* string, vga_color_t fg,
		vga_color_t bg) {
	while(*string) {
		vga_console_putc_color(*string, fg, bg);
		++string;
	}
}


void vga_console_init() {
	// there is other modes but this one more or less default
	__vga_memory = (vga_cell_t *) 0xb8000;
	__vga_height = 25;
	__vga_width = 80;
	vga_set_fg(vga_grey);
	vga_set_bg(vga_black);
}


void vga_clear_screen() {
	vga_cell_t empty = __vga_cell(' ', __current_fg, __current_bg);
	memset_16((char *)__vga_memory, address_of(empty),
		__vga_height*__vga_width);
}

static vga_cell_t __vga_cell(char symbol, vga_color_t fg, vga_color_t bg) {
	vga_cell_t cell;
	cell.bg = bg;
	cell.fg = fg;
	cell.ch = symbol;
	return cell;
}


static void __vga_cursor_next() {
	++__vga_pos_column;
	if (__vga_pos_column == __vga_width) {
		__vga_linebreak();
	}
}

static void __vga_newline() {
	if (++__vga_pos_line == __vga_height) {
		__vga_pos_line = __vga_height - 1;
		__vga_scroll();
	}
}

static void __vga_linebreak() {
	__vga_newline();
	__vga_carriage_return();
}

static void __vga_scroll() {
	for (int i = 0; i < __vga_height - 1; ++i) {
		memcpy((char *)(__vga_memory + i * __vga_width),
			(char *)(__vga_memory + (i + 1) * __vga_width),
			(__vga_width * sizeof(vga_cell_t)));
	}
	uint16_t zero = 0;
	memset_16((char *)(__vga_memory + (__vga_height - 1) * __vga_width),
		(char *) &zero, __vga_width);
}

static void __vga_carriage_return() {
	__vga_pos_column = 0;
}
