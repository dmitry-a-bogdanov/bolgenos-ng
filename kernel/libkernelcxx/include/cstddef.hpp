#pragma once

#if !defined(__cplusplus)
#error This is C++-only header
#endif

#include "impl/cstddef/byte.hpp"

namespace lib {

#include <stddef.h> // libgcc header

using nullptr_t = decltype(nullptr);

} // namespace lib

using lib::nullptr_t;
using lib::ptrdiff_t;
using lib::size_t;
