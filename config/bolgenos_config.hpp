#pragma once

#include <cstdint>
#include <cstddef>

namespace config
{

constexpr inline char BOLGENOS_NG_VERSION[] = "0.0.4";

constexpr inline std::size_t HZ = 1000;

constexpr inline bool VERBOSE_TIMER_INTERRUPT = false;

constexpr inline std::size_t PAGE_SIZE = 4096;

constexpr inline std::size_t STACK_PAGES = 1024;

constexpr inline std::size_t STACK_SIZE = STACK_PAGES * STACK_PAGES;

constexpr inline std::size_t STACK_CANARY_PAGES = 1;

}
