#pragma once

#include "type_traits.hpp"

namespace lib {

namespace detail {
template< class T, class U >
concept SameHelper = lib::is_same_v<T, U>;
}

template< class T, class U >
concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

}