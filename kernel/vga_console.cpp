#include <bolgenos-ng/vga_console.hpp>

#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/stdtypes.hpp>


namespace {


struct __attribute__((packed)) cell_t {
	cell_t(char ch, vga_console::color_t fg, vga_console::color_t bg)
		: ch_(ch), fg_(fg), bg_(bg) {}
	char ch_:8;
	uint8_t fg_:4;
	uint8_t bg_:4;
};
static_assert(sizeof(cell_t) == 2, "cell_t has incorrect size!");


cell_t *iomem = reinterpret_cast<cell_t *>(0xb8000);


vga_console::color_t global_bg = vga_console::color_t::black;
vga_console::color_t global_fg = vga_console::color_t::grey;


int screen_height = 25, screen_width = 80;
int cursor_line = 0, cursor_column = 0;


const char LF = '\n';
const char CR = '\r';


void cursor_next();
void line_break();
void new_line();
void scroll();
void carriage_return();


cell_t *cursor_address();


} // namespace





void vga_console::set_bg(color_t bg) {
	global_bg = bg;
}


vga_console::color_t vga_console::get_bg() {
	return global_bg;
}


void vga_console::set_fg(color_t fg) {
	global_fg = fg;
}


vga_console::color_t vga_console::get_fg() {
	return global_fg;
}


void vga_console::putc_color(char symbol, color_t fg, color_t bg) {
	switch (symbol) {
	case LF:
		line_break();
		break;
	case CR:
		carriage_return();
		break;
	default:
		{
			cell_t cell = cell_t(symbol, fg, bg);
			*cursor_address() = cell;
			cursor_next();
		}
	}
}


void vga_console::putc(char symbol) {
	putc_color(symbol, global_fg, global_bg);
}


void vga_console::puts(const char* string) {
	puts_color(string, global_fg, global_bg);
}


void vga_console::puts_color(const char* string, color_t fg,
		color_t bg) {
	while(*string) {
		vga_console::putc_color(*string, fg, bg);
		++string;
	}
}


void vga_console::clear_screen() {
	cell_t empty = cell_t(' ', global_fg, global_bg);
	memset_16(reinterpret_cast<char *>(iomem),
		reinterpret_cast<char *>(address_of(empty)),
		screen_height*screen_width);
}


namespace {

/**
* Move cursor to the next position.
*/
void cursor_next() {
	++cursor_column;
	if (cursor_column == screen_width) {
		line_break();
	}
}


/**
* Do new line.
*/
void new_line() {
	if (++cursor_line == screen_height) {
		cursor_line = screen_height - 1;
		scroll();
	}
}


/**
* Do line break.
*/
void line_break() {
	new_line();
	carriage_return();
}


/**
* Scrool vga display on one line.
*/
void scroll() {
	for (int i = 0; i < screen_height - 1; ++i) {
		memcpy((char *)(iomem + i * screen_width),
			(char *)(iomem + (i + 1) * screen_width),
			(screen_width * sizeof(cell_t)));
	}
	uint16_t zero = 0;
	memset_16((char *)(iomem + (screen_height - 1) * screen_width),
		(char *) &zero, screen_width);
}


/**
* Do carriage return.
*/
void carriage_return() {
	cursor_column = 0;
}


/**
* Get pointer to cursor cell.
*/
cell_t *cursor_address() {
	return iomem + cursor_line*screen_width + cursor_column;
}


} // namespace
