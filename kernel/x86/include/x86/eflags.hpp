#pragma once

#include <cstdint.hpp>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/asm.hpp>

namespace x86 {



struct [[gnu::packed]] EFlags
{
	#define reserved macro_concat(_reserved_, __LINE__)
	bool carry: 1;
	bool reserved: 1;
	bool parity: 1;
	bool reserved: 1;
	bool auxiliary_carry: 1;
	bool reserved: 1;
	bool zero: 1;
	bool sign: 1;
	bool trap: 1;
	bool interrupts: 1;
	bool direction: 1;
	bool overflow: 1;
	ProtectionRing io_privilege_level: 2;
	bool nested_task: 1;
	bool reserved: 1;

	bool resume: 1;
	bool virtual_8086: 1;
	bool alignment_check: 1;
	bool virtual_interrupt: 1;
	bool virtual_interrupt_pending: 1;
	bool cpuid_available: 1;
	uint32_t reserved: 10;
	#undef reserved
};

}

