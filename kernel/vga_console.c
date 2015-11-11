#include <bolgenos-ng/vga_console.h>

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>

static uint16_t *vga_console_memory;
static int vga_height, vga_width;
static int8_t vga_bg = 0, vga_fg = 15;
static int vga_cursor_line = 0, vga_cursor_column = 0;

#define vga_offset (vga_cursor_line*vga_width + vga_cursor_column)
#define vga_cursor_addr (vga_console_memory + vga_offset)

#define low_bits_4(x) (x & 0xf)
#define vga_color ((low_bits_4(vga_bg) << 4) | low_bits_4(vga_fg))

#define VGA_NEWLINE_SYMBOL '\n'
#define VGA_CARRIAGE_RETURN_SYMBOL '\r'

uint16_t vga_char(char symbol) {
	return (vga_color << 8) | symbol;
}

static void vga_cursor_next();
static void vga_linebreak();
static void vga_newline();
static void vga_carriage_return();

void vga_console_putc(char symbol) {
	switch (symbol) {
	case VGA_NEWLINE_SYMBOL:
		vga_linebreak();
		break;
	case VGA_CARRIAGE_RETURN_SYMBOL:
		vga_carriage_return();
		break;
	default:
		{
			uint16_t vga_symbol;
			vga_symbol = vga_char(symbol);
			write_16(vga_cursor_addr, vga_symbol);
			vga_cursor_next();
		}
	}
}

void vga_console_puts(const char* string) {
	while(*string) {
		vga_console_putc(*string);
		++string;
	}
}


void vga_console_init() {
	// there is other modes but this one more or less default
	vga_console_memory = (uint16_t *) 0xb8000;
	vga_height = 25;
	vga_width = 80;
}


void vga_clear_screen() {
	uint16_t empty = vga_char(' ');
	memset_16(vga_console_memory, empty, vga_height*vga_width);
}




static void vga_cursor_next() {
	++vga_cursor_column;
	if (vga_cursor_column == vga_width) {
		vga_linebreak();
	}
}

static void vga_newline() {
	if (++vga_cursor_line == vga_height) {
		vga_cursor_line = 0;
	}
}

static void vga_linebreak() {
	vga_newline();
	vga_carriage_return();
}

static void vga_carriage_return() {
	vga_cursor_column = 0;
}
