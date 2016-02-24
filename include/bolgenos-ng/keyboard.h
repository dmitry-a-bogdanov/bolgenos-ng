#pragma once

/** \brief Keyboard's key.
*
* Enum holds key on keyboard and additional internal value to make coding
*	easier.
*/
typedef enum {
	__kb_key_none		= 0x00, /*!< Invalid key */

	kb_key_f1,
	kb_key_f2,
	kb_key_f3,
	kb_key_f4,
	kb_key_f5,
	kb_key_f6,
	kb_key_f7,
	kb_key_f8,
	kb_key_f9,
	kb_key_f10,
	kb_key_f11,
	kb_key_f12,

	kb_key_tab,
	kb_key_backspace,

	kb_key_lalt,
	kb_key_lshift,
	kb_key_lctrl,

	kb_key_ralt,
	kb_key_rshift,
	kb_key_rctrl,

	kb_key_back_tick,

	kb_key_0,
	kb_key_1,
	kb_key_2,
	kb_key_3,
	kb_key_4,
	kb_key_5,
	kb_key_6,
	kb_key_7,
	kb_key_8,
	kb_key_9,

	kb_key_a,
	kb_key_b,
	kb_key_c,
	kb_key_d,
	kb_key_e,
	kb_key_f,
	kb_key_g,
	kb_key_h,
	kb_key_i,
	kb_key_j,
	kb_key_k,
	kb_key_l,
	kb_key_m,
	kb_key_n,
	kb_key_o,
	kb_key_p,
	kb_key_q,
	kb_key_r,
	kb_key_s,
	kb_key_t,
	kb_key_u,
	kb_key_v,
	kb_key_w,
	kb_key_x,
	kb_key_y,
	kb_key_z,

	kb_key_space,
	kb_key_comma,
	kb_key_dot,
	kb_key_slash,
	kb_key_semicolon,
	kb_key_minus,
	kb_key_apos,
	kb_key_sqbracketl,
	kb_key_equal,
	kb_key_enter,
	kb_key_sqbracketr,
	kb_key_backslash,


	kb_key_num_0,
	kb_key_num_1,
	kb_key_num_2,
	kb_key_num_3,
	kb_key_num_4,
	kb_key_num_5,
	kb_key_num_6,
	kb_key_num_7,
	kb_key_num_8,
	kb_key_num_9,
	kb_key_num_dot,
	kb_key_num_plus,
	kb_key_num_minus,
	kb_key_num_star,
	kb_key_num_slash,
	kb_key_num_enter,

	kb_key_escape,

	kb_key_capslock,
	kb_key_numlock,
	kb_key_scrolllock,

	kb_key_acpi_sleep,
	kb_key_acpi_power,
	kb_key_acpi_wake,

	kb_key_cursor_up,
	kb_key_cursor_down,
	kb_key_cursor_right,
	kb_key_cursor_left,

	kb_key_mm_www_search,
	kb_key_mm_prev_track,
	kb_key_mm_www_fav,
	kb_key_mm_www_refresh,
	kb_key_mm_voldown,
	kb_key_mm_mute,
	kb_key_mm_www_stop,
	kb_key_mm_calc,
	kb_key_mm_www_forward,
	kb_key_mm_vol_up,
	kb_key_mm_playpause,
	kb_key_mm_www_back,
	kb_key_mm_www_home,
	kb_key_mm_stop,
	kb_key_mm_mycomp,
	kb_key_mm_email,
	kb_key_mm_next_track,
	kb_key_mm_select,

	kb_key_end,
	kb_key_home,
	kb_key_page_down,
	kb_key_page_up,

	kb_key_lgui,
	kb_key_rgui,

	kb_key_apps,
	kb_key_insert,
	kb_key_delete,
	kb_key_print_screen,
	kb_key_pause,

	__kb_key_max /*!< Number of values in this enum */
} kb_key;

kb_key& operator++(kb_key& key);


/**
* \brief Key released flag.
*
* Flag for kb_keys_pressed used to mark key is not pressed or released.
*/
#define KEY_RELEASED			0x0


/**
* \brief Key pressed flag.
*
* Flag for kb_keys_pressed used to mark key as pressed.
*/
#define KEY_PRESSED			0x1


/**
* \brief Key status array
*
* Array shows which keys are pressed and released. For differentiating these
*	statuses KEY_PRESSED and KEY_RELEASED constants are used.
*/
extern char kb_keys_pressed[__kb_key_max];


/**
* \brief React to pressed keys with VGA display
*
* Function prints to VGA display data that is got from keyboard.
*/
void kb_print_to_vga();

