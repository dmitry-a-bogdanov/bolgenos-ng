#pragma once

#include <bolgenos-ng/stdtypes.hpp>


/// \brief Handle recevied byte from PS/2 keyboard.
///
/// Put byte received from PS/2 keyboard to keyboard state machine.
void kbd_put_byte(uint8_t byte);

