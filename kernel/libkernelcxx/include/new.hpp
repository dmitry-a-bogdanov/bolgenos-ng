
#include <cstddef.hpp>

/// Empty placement new.
[[nodiscard]] void *operator new(size_t size, void *address);
