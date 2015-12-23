#include <bolgenos-ng/vga_console.h>

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>


#define LF						'\n'
#define CR						'\r'


typedef struct __attribute__((packed)) {
	char ch:8;
	vga_color_t fg:4;
	vga_color_t bg:4;
} vga_cell_t;

assert_type_size(vga_cell_t, 2);

#define VGA_CELL_INITIALIZER(sym, f, b)				{	\
		.ch = sym,						\
		.fg = f,						\
		.bg = b,						\
	}

static vga_cell_t *vga_iomem;
static vga_color_t global_bg;
static vga_color_t global_fg;

static int screen_height, screen_width;
static int cursor_line = 0, cursor_column = 0;

#define cursor_offset (cursor_line*screen_width + cursor_column)
#define cursor_address (vga_iomem + cursor_offset)

static void cursor_next();
static void line_break();
static void new_line();
static void scroll();
static void carriage_return();

void vga_set_bg(vga_color_t bg) {
	global_bg = bg;
}

vga_color_t vga_get_bg() {
	return global_bg;
}

void vga_set_fg(vga_color_t fg) {
	global_fg = fg;
}

vga_color_t vga_get_fg() {
	return global_fg;
}


void vga_console_putc_color(char symbol, vga_color_t fg,
		vga_color_t bg) {
	switch (symbol) {
	case LF:
		line_break();
		break;
	case CR:
		carriage_return();
		break;
	default:
		{
			vga_cell_t cell = VGA_CELL_INITIALIZER(symbol, fg, bg);
			write_16((char *) cursor_address, address_of(cell));
			cursor_next();
		}
	}
}

void vga_console_putc(char symbol) {
	vga_console_putc_color(symbol, global_fg, global_bg);
}

void vga_console_puts(const char* string) {
	vga_console_puts_color(string, global_fg, global_bg);
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
	vga_iomem = (vga_cell_t *) 0xb8000;
	screen_height = 25;
	screen_width = 80;
	vga_set_fg(vga_grey);
	vga_set_bg(vga_black);
}


void vga_clear_screen() {
	vga_cell_t empty = VGA_CELL_INITIALIZER(' ', global_fg, global_bg);
	memset_16((char *)vga_iomem, address_of(empty),
		screen_height*screen_width);
}


/**
* Move cursor to the next position.
*/
static void cursor_next() {
	++cursor_column;
	if (cursor_column == screen_width) {
		line_break();
	}
}


/**
* Do new line.
*/
static void new_line() {
	if (++cursor_line == screen_height) {
		cursor_line = screen_height - 1;
		scroll();
	}
}


/**
* Do line break.
*/
static void line_break() {
	new_line();
	carriage_return();
}


/**
* Scrool vga display on one line.
*/
static void scroll() {
	for (int i = 0; i < screen_height - 1; ++i) {
		memcpy((char *)(vga_iomem + i * screen_width),
			(char *)(vga_iomem + (i + 1) * screen_width),
			(screen_width * sizeof(vga_cell_t)));
	}
	uint16_t zero = 0;
	memset_16((char *)(vga_iomem + (screen_height - 1) * screen_width),
		(char *) &zero, screen_width);
}


/**
* Do carriage return.
*/
static void carriage_return() {
	cursor_column = 0;
}
