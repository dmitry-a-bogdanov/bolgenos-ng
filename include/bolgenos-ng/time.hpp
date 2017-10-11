#pragma once

#include <cstddef>
#include <cstdint>

namespace bolgenos
{

constexpr inline std::size_t HZ = 1000;
extern volatile uint32_t jiffies;

void sleep_ms(uint32_t ms);
uint32_t ms_to_ticks(uint32_t ms);

}
