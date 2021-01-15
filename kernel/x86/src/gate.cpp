#include <x86/gate.hpp>

#include <type_traits.hpp>

static_assert(sizeof(x86::Gate) == 8);
static_assert(lib::is_standard_layout_v<x86::Gate>);
