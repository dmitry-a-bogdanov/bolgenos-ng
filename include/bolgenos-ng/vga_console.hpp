#pragma once

#include <bolgenos-ng/stdtypes>


namespace vga_console {

/**
* \brief All colors supported by VGA console.
*
* Enum hold all colors that are supported by VGA console.
*/
enum color_t: uint8_t {
	black			= 0,
	blue			= 1,
	green			= 2,
	cyan			= 3,
	red			= 4,
	magenta			= 5,
	brown			= 6,
	grey			= 7,
	dark_grey		= 8,
	bright_blue		= 9,
	bright_green		= 10,
	bright_cyan		= 11,
	bright_red		= 12,
	bright_magenta		= 13,
	yellow			= 14,
	white			= 15
};


/**
* \brief Set background.
*
* Set background color to specified value.
* \param bg Background color to be set.
*/
void set_bg(color_t bg);


/**
* \brief Get backgroud.
*
* Get background color.
* \return Current backgrond color.
*/
color_t get_bg();


/**
* \brief Set foreground.
*
* Set foreground color to specified value.
* \param fg Foreground color to be set.
*/
void set_fg(color_t fg);


/**
* \brief Get foregroud.
*
* Get foreground color.
* \return Current foregrond color.
*/
color_t get_fg();


/**
* \brief Clear screen.
*
* Function clears VGA console
*/
void clear_screen();


/**
* \brief Print character.
*
* Function prints one character to VGA console using current global background
*	and foreground values.
* \param c Symbol to be printed.
*/
void putc(char c);


/**
* \brief Print string.
*
* Function prints string to VGA console using current global background and
*	foreground values.
* \param str String to be printed.
*/
void puts(const char* str);


/**
* \brief Print character.
*
* Function prints one character to VGA console using specified background and
*	foreground colors.
* \param c Symbol to be printed.
* \param fg Foreground color.
* \param bg Background color.
*/
void putc_color(char c, color_t fg, color_t bg);


/**
* \brief Print string.
*
* Function prints string to VGA console using specified background and
*	foreground colors.
* \param str String to be printed.
* \param fg Foreground color.
* \param bg Background color.
*/
void puts_color(const char* str, color_t fg, color_t bg);

} // namespace vga_console
