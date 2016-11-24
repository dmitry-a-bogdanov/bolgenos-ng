#pragma once

namespace bolgenos_testing {

inline
uint32_t lzcnt(uint32_t value)
{
	uint32_t retval;
	asm("lzcntl %1, %0": "=a"(retval) : "b"(value));
	return retval;
}

} // namespace bolgenos_testing
