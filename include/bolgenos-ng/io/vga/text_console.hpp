#pragma once

#include "types.hpp"

namespace bolgenos { namespace io { namespace vga
{

class VideoMemory;

class TextConsole
{
public:

	static TextConsole *instance();

	struct position_t
	{
		signed char row;
		signed char col;
	};

	void clear_screen();

	color_t fg() const;
	color_t fg(color_t foreground);


	color_t bg() const;
	color_t bg(color_t background);


	void putc(char_t chr);
	void putc(char c);


	char_t null_char() const;

	void puts(const char *s);

protected:

    struct sentry
    {
        explicit sentry(TextConsole *display)
            : _display{ display }
        {
        }


        sentry(const sentry&) = delete;
        sentry& operator =(const sentry&) = delete;

        ~sentry()
        {
            _display->update_cursor();
        }

        TextConsole* _display;
    };


	void next_char();

	void scroll();

	void newline();

	void carriage_return();

	void linebreak();

	char_t& current_char();

	void update_cursor();


private:

	const static std::uintptr_t _base_address;

	TextConsole(color_t fg_color=color_t::green, color_t bg_color=color_t::black, bool clear=false);

	VideoMemory *_memory;
	position_t _cursor_position;
	color_t _fg_color;
	color_t _bg_color;
};
// class Display


//using streambuf_t = std::streambuf<bolgenos::io::vga::char_t>;

}
} }
// namespace bolgenos::io::vga

