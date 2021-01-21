#pragma once

#include "type_traits.hpp"

namespace lib {

namespace detail {
template< class T, class U >
concept SameHelper = lib::is_same_v<T, U>;
}

template< class T, class U >
concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;


template<class T>
concept Enum = lib::is_enum_v<T>;

template<class U, class T>
concept EnumFrom = lib::is_enum_v<U> && lib::is_same_v<lib::underlying_type_t<U>, T>;

}