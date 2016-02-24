#include "ps2_keyboard_sm.hpp"

#include <bolgenos-ng/keyboard.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.hpp>

#define __st_debug_enable				(0)

#define st_dbg(format, ...)						\
	do {								\
		if (__st_debug_enable) {				\
			printk(format, ##__VA_ARGS__);			\
		}							\
	} while(0)

#define dbg_enter() st_dbg(" entering state '%s'; ", __func__)
#define dbg_exit() st_dbg(" exiting state '%s'; ", __func__)

char codeset2_ascii[256] = {
	  0,   0,    0,   0,      0,    0,    0, 0,		// 0x00 - 0x07
	  0,   0,    0,   0,      0, '\t',  '`', 0,		// 0x08 - 0x0f
	  0,   0,    0,   0,      0,  'q',  '1', 0,		// 0x10 - 0x17
	  0,   0,  'z', 's',    'a',  'w',  '2', 0,		// 0x18 - 0x1f
	  0, 'c',  'x', 'd',    'e',  '4',  '3', 0,		// 0x20 - 0x27
	  0, ' ',  'v', 'f',    't',  'r',  '5', 0,		// 0x28 - 0x2f
	  0, 'n',  'b', 'h',    'g',  'y',  '6', 0,		// 0x30 - 0x37
	  0,   0,  'm', 'j',    'u',  '7',  '8', 0,		// 0x38 - 0x3f
	  0, ',',  'k', 'i',    'o',  '0',  '9', 0,		// 0x40 - 0x47
	  0, '.',  '/', 'l',    ';',  'p',  '-', 0,		// 0x48 - 0x4f
	  0,   0, '\'',   0,    '[',  '=',    0, 0,		// 0x50 - 0x57
	  0,   0, '\n', ']',      0, '\\',    0, 0,		// 0x58 - 0x5f
	  0,   0,    0,   0,      0,    0, '\b', 0,		// 0x60 - 0x67
	  0, '1',    0, '4',    '7',    0,    0, 0,		// 0x68 - 0x6f
	'0', '.',  '2', '5',    '6',  '8',    0, 0,		// 0x70 - 0x77
	  0, '+',  '3', '-',    '*',  '9',    0, 0,		// 0x78 - 0x7f

	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

struct kbd_dev;
struct kbd_state;
typedef struct kbd_dev kbd_dev_t;
typedef struct kbd_state kbd_state_t;

typedef enum {
	stf_ret_done,
	stf_ret_continue,
} stf_ret_t;

typedef stf_ret_t (*stf_t)(kbd_dev_t *, uint8_t); // state function

struct kbd_state {
	stf_t func;
};

struct kbd_dev {
	kbd_state_t *state;
};

static stf_ret_t stf_wait_for_input(kbd_dev_t *dev, uint8_t byte);
static stf_ret_t stf_1b_key_pressed(kbd_dev_t *dev, uint8_t byte);
static stf_ret_t stf_1b_key_released(kbd_dev_t *dev, uint8_t byte);
static stf_ret_t stf_print(kbd_dev_t *dev, uint8_t byte);
static stf_ret_t stf_error(kbd_dev_t *dev, uint8_t byte);

static kbd_state_t st_wait_for_input = {
	.func = &stf_wait_for_input
};

static kbd_state_t st_1b_key_pressed = {
	.func = &stf_1b_key_pressed
};

static kbd_state_t st_1b_key_released = {
	.func = &stf_1b_key_released
};

static kbd_state_t st_print = {
	.func = &stf_print
};

static kbd_state_t st_error = {
	.func = &stf_error
};



static stf_ret_t stf_error(kbd_dev_t *dev, uint8_t byte) {
	dbg_enter();
	printk("PS/2 keyboard: unexpected byte from controller '%lu'\n",
		(long unsigned) byte);
	dev->state = &st_wait_for_input;
	dbg_exit();
	return stf_ret_done;
}

static char codeset2_1b_translation_table[256] = {
	// 0x00
	0, kb_key_f9, 0, kb_key_f5,
	kb_key_f3, kb_key_f1, kb_key_f2, kb_key_f12,
	0, kb_key_f10, kb_key_f8, kb_key_f6,
	0, kb_key_tab,  kb_key_back_tick, 0,


	// 0x10
	0, kb_key_lalt, kb_key_lshift, 0,
	kb_key_lctrl, kb_key_q, kb_key_1, 0,
	0, 0, kb_key_z, kb_key_s,
	kb_key_a, kb_key_w, kb_key_2, 0,

	// 0x20
	0, kb_key_c, kb_key_x, kb_key_d,
	kb_key_e, kb_key_4, kb_key_3, 0,
	0, kb_key_space, kb_key_v, kb_key_f,
	kb_key_t, kb_key_r, kb_key_5, 0,


	// 0x30
	0, kb_key_n, kb_key_b, kb_key_h,
	kb_key_g, kb_key_y, kb_key_6, 0,
	0,   0,  kb_key_m, kb_key_j,
	kb_key_u, kb_key_7, kb_key_8, 0,


	// 0x40
	0, kb_key_comma,  kb_key_k, kb_key_i,
	kb_key_o, kb_key_0, kb_key_9, 0,
	0, kb_key_dot, kb_key_slash, kb_key_l,
	kb_key_semicolon, kb_key_p, kb_key_minus, 0,

	// 0x50
	0,   0, kb_key_apos, 0,
	kb_key_sqbracketl, kb_key_equal, 0, 0,
	kb_key_capslock, kb_key_rshift, kb_key_enter, kb_key_sqbracketr,
	0, kb_key_backslash, 0, 0,

	// 0x60
	0, 0, 0, 0,
	0, 0, kb_key_backspace, 0,
	0, kb_key_num_1, 0, kb_key_num_4,
	kb_key_num_7, 0, 0, 0,

	// 0x70
	kb_key_num_0, kb_key_num_dot, kb_key_num_2, kb_key_num_5,
	kb_key_num_6, kb_key_num_8, kb_key_escape, kb_key_numlock,
	kb_key_f11, kb_key_num_plus, kb_key_num_3, kb_key_num_minus,
	kb_key_num_star, kb_key_num_9, kb_key_scrolllock, 0,

	// 0x80
	0, 0, 0, kb_key_f7,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};
#define codeset2_1b_release_byte			(0xf0)

static stf_ret_t stf_wait_for_input(kbd_dev_t *dev, uint8_t byte) {
	dbg_enter();
	if (byte == codeset2_1b_release_byte) {
		dev->state = &st_1b_key_released;
	} else if (codeset2_1b_translation_table[byte] != __kb_key_none) {
		dev->state = &st_1b_key_pressed;
	} else {
		dev->state = &st_error;
	}
	dbg_exit();
	return stf_ret_continue;
}

static stf_ret_t stf_1b_key_pressed(kbd_dev_t *dev, uint8_t byte) {
	dbg_enter();
	int key = codeset2_1b_translation_table[byte];
	if (key != __kb_key_none) {
		if (kb_keys_pressed[key] != KEY_PRESSED) {
			kb_keys_pressed[key] = KEY_PRESSED;
			dev->state = &st_print;
		} else {
			dev->state = &st_wait_for_input;
		}
	}
	dbg_exit();
	return stf_ret_continue;
}

static stf_ret_t stf_1b_key_released(kbd_dev_t *dev, uint8_t byte) {
	dbg_enter();
	if (byte == codeset2_1b_release_byte) {
		// got 0xf0. wait for next byte
		dev->state = &st_1b_key_released;
		dbg_exit();
		return stf_ret_done;
	}
	int key = codeset2_1b_translation_table[byte];
	if (key != __kb_key_none) {
		kb_keys_pressed[key] = KEY_RELEASED;
		dev->state = &st_wait_for_input;
		dbg_exit();
		return stf_ret_done;
	} else {
		dev->state = &st_error;
		dbg_exit();
		return stf_ret_continue;
	}
}

static stf_ret_t stf_print(kbd_dev_t *dev, uint8_t byte) {
	(void) byte;
	dbg_enter();
	kb_print_to_vga();
	dev->state = &st_wait_for_input;
	dbg_exit();
	return stf_ret_done;
}

kbd_dev_t default_keyboard = {
	.state = &st_wait_for_input
};

void kbd_put_byte(uint8_t byte) {
	while (default_keyboard.state->func(&default_keyboard, byte) != stf_ret_done) ;
}

