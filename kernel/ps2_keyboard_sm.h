#ifndef __BOLGENOS_NG__PS2_KEYBOARD_SM_H__
#define __BOLGENOS_NG__PS2_KEYBOARD_SM_H__ (1)

#include <bolgenos-ng/int_types.h>

#define ps2_keyboard_keys		(256)

extern char pressed_keys[ps2_keyboard_keys];

void ps2_kb_sm_put_byte(uint8_t byte);

#endif // __BOLGENOS_NG__PS2_KEYBOARD_SM_H__
