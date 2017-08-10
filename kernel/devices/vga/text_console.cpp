#include <bolgenos-ng/io/vga/text_console.hpp>

#include <new>
#include <utility>
#include <bolgenos-ng/asm.hpp>

#include <bolgenos-ng/io/vga/video_memory.hpp>

using namespace bolgenos::io::vga;


TextConsole::TextConsole(color_t fg_color, color_t bg_color, bool clear)
	: _memory{new (reinterpret_cast<void *>(_base_address)) VideoMemory},
	_cursor_position{0, 0},
	_fg_color(fg_color),
	_bg_color(bg_color)
{
	if (clear)
	{
		clear_screen();
	}
}

TextConsole* TextConsole::instance()
{
	static TextConsole display;
	return &display;
}


void TextConsole::clear_screen()
{
	for (auto& row: _memory->video_buffer)
	{
		for (auto& vga_char: row)
		{
			vga_char = null_char();
		}
	}
	_cursor_position = {0, 0};
}


color_t TextConsole::fg() const
{
	return _fg_color;
}


color_t TextConsole::fg(color_t foreground)
{
	std::swap(foreground, _fg_color);
	return foreground;
}


color_t TextConsole::bg() const
{
	return _bg_color;
}


color_t TextConsole::bg(color_t background)
{
	std::swap(background, _bg_color);
	return _bg_color;
}


void TextConsole::putc(char_t chr)
{
	sentry(this);

	switch (chr.ch())
	{
	case '\n':
		linebreak();
		break;
	case '\r':
		carriage_return();
		break;
	default:
		current_char() = chr;
		next_char();
	}
}


void TextConsole::putc(const char c)
{
	putc(char_t{c, _fg_color, _bg_color});
}


char_t TextConsole::null_char() const
{
	return char_t{'\0', _fg_color, _bg_color};
}


void TextConsole::puts(const char *s)
{
	while(*s)
	{
		putc(*s++);
	}
}


void TextConsole::next_char()
{
	++_cursor_position.col;
	if (_cursor_position.col == _memory->COLUMNS) {
		linebreak();
	}
}


void TextConsole::scroll()
{
	auto& video_buffer = _memory->video_buffer;
	for (std::size_t r = 0; r != _memory->ROWS - 1; ++r)
	{
		for (std::size_t c = 0; c != _memory->COLUMNS; ++c)
		{
			video_buffer[r][c] = video_buffer[r + 1][c];
		}
	}

	for (std::size_t c = 0; c != _memory->COLUMNS; ++c)
	{
		video_buffer[_memory->ROWS - 1][c] = null_char();
	}
}


void TextConsole::newline()
{
	if (++_cursor_position.row == VideoMemory::ROWS)
	{
		_cursor_position.row = _memory->ROWS - 1;
		scroll();
	}
}


void TextConsole::carriage_return()
{
	_cursor_position.col = 0;
}


void TextConsole::linebreak()
{
	newline();
	carriage_return();
}


char_t& TextConsole::current_char()
{
	return _memory->video_buffer[_cursor_position.row][_cursor_position.col];
}


void TextConsole::update_cursor()
{
	//unsigned position= (_cursor_position.row*80) + _cursor_position.col;
	unsigned int position = _cursor_position.row * _memory->COLUMNS + _cursor_position.col + 1;
	// cursor LOW port to vga INDEX register
	outb(0x3D4, 0x0F);
	outb(0x3D5, static_cast<std::uint8_t>(position & 0xFF));
	// cursor HIGH port to vga INDEX register
	outb(0x3D4, 0x0E);
	outb(0x3D5, static_cast<std::uint8_t>((position>>8)&0xFF));
}




const std::uintptr_t TextConsole::_base_address = 0xb8000;


