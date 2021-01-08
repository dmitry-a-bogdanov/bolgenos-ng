#pragma once

#if !defined(__cplusplus)
#error This is C++-only header
#endif

namespace lib {

#include <stdint.h> // libgcc header

} // namespace lib

using lib::int8_t;
using lib::int16_t;
using lib::int32_t;


using lib::uint8_t;
using lib::uint16_t;
using lib::uint32_t;
