#ifndef __BOLGENOS_NG__VGA_CONSOLE_H__
#define __BOLGENOS_NG__VGA_CONSOLE_H__

/**
* \brief All colors supported by VGA console.
*
* Enum hold all colors that are supported by VGA console.
*/
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


/**
* \brief Initilize VGA Console subsystem.
*
* The fuction initializes internal structures of VGA console subsystems.
*/
void vga_console_init();


/**
* \brief Set background.
*
* Set background color to specified value.
* \param bg Background color to be set.
*/
void vga_set_bg(vga_color_t bg);


/**
* \brief Get backgroud.
*
* Get background color.
* \return Current backgrond color.
*/
vga_color_t vga_get_bg();


/**
* \brief Set foreground.
*
* Set foreground color to specified value.
* \param fg Foreground color to be set.
*/
void vga_set_fg(vga_color_t fg);


/**
* \brief Get foregroud.
*
* Get foreground color.
* \return Current foregrond color.
*/
vga_color_t vga_get_fg();


/**
* \brief Clear screen.
*
* Function clears VGA console
*/
void vga_clear_screen();


/**
* \brief Print character.
*
* Function prints one character to VGA console using current global background
*	and foreground values.
* \param c Symbol to be printed.
*/
void vga_console_putc(char c);


/**
* \brief Print string.
*
* Function prints string to VGA console using current global background and
*	foreground values.
* \param str String to be printed.
*/
void vga_console_puts(const char* str);


/**
* \brief Print character.
*
* Function prints one character to VGA console using specified background and
*	foreground colors.
* \param c Symbol to be printed.
* \param fg Foreground color.
* \param bg Background color.
*/
void vga_console_putc_color(char c, vga_color_t fg, vga_color_t bg);


/**
* \brief Print string.
*
* Function prints string to VGA console using specified background and
*	foreground colors.
* \param str String to be printed.
* \param fg Foreground color.
* \param bg Background color.
*/
void vga_console_puts_color(const char* str, vga_color_t fg, vga_color_t bg);

#endif // __BOLGENOS_NG__VGA_CONSOLE_H__
