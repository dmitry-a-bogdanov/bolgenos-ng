#pragma once

#include <cstdint>
#include <type_traits>

namespace bolgenos
{
namespace io
{
namespace vga
{



enum color_t: std::uint8_t
{
	black           = 0,
	blue            = 1,
	green           = 2,
	cyan            = 3,
	red             = 4,
	magenta         = 5,
	brown           = 6,
	grey            = 7,
	dark_grey       = 8,
	bright_blue     = 9,
	bright_green    = 10,
	bright_cyan     = 11,
	bright_red      = 12,
	bright_magenta  = 13,
	yellow          = 14,
	white           = 15,
};


class [[gnu::packed]] char_t
{
public:
	constexpr char_t() = default;


	constexpr char_t(char character, color_t foreground, color_t background)
		: _ch(character),
		_color((foreground << _fg_offset)|(background << _bg_offset))
	{
	}


	constexpr char ch() const
	{
		return _ch;
	}


	constexpr char ch(char character)
	{
		auto prev = ch();
		_ch = character;
		return prev;
	}


	constexpr color_t fg() const
	{
		return static_cast<color_t>((_color & _fg_mask) >> _fg_offset);
	}


	constexpr color_t fg(color_t foreground)
	{
		auto prev = fg();
		_color = (_color & _bg_mask) | (foreground << _fg_offset);
		return prev;
	}


	constexpr color_t bg() const
	{
		return static_cast<color_t>((_color & _bg_mask) >> _bg_offset);
	}


	constexpr color_t bg(color_t background)
	{
		auto prev = bg();
		_color = (_color & _fg_mask) | (static_cast<color_type>(background) << _bg_offset);
		return prev;
	}

	private:
	using color_type = std::undelying_type_t<color_t>;


	char _ch{ 0 };
	std::uint8_t _color{ 0 };


	inline static constexpr std::uint8_t _bg_mask = 0xf0;
	inline static constexpr std::uint8_t _bg_offset = 4;
	inline static constexpr std::uint8_t _fg_mask = 0x0f;
	inline static constexpr std::uint8_t _fg_offset = 0;
};
// class char_t


static_assert(sizeof(char_t) == 2, "VGA cell must be 2 bytes");



}
}
}
