#pragma once

#include "add_reference.hpp"

namespace lib {

template<class T>
typename lib::add_rvalue_reference<T>::type declval() noexcept;

}