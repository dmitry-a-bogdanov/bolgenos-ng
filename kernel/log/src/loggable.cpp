#include <loggable.hpp>

#include <type_traits.hpp>

static_assert(lib::is_empty_v<Loggable("test")>);

