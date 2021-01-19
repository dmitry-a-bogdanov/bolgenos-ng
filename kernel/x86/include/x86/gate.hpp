#pragma once

#include <cstdint.hpp>
#include "segment_flags.hpp"
#include "gdt.hpp"

#include <mem_utils.hpp>

namespace x86 {

enum class GateType: uint8_t {
	task		= 0x5,
	interrupt	= 0x6,
	trap		= 0x7,
};

struct [[gnu::packed]] Gate {
	explicit constexpr Gate(void (*func)(), GateType type)
		: Gate((lib::uintptr_t) func, type)
	{}

	explicit constexpr Gate(lib::uintptr_t func,
				GateType kind)
		: offset_00_15_(shiftmask(func, 0, 0xffff)),
		  _gate_kind{kind},
		  offset_16_31_(shiftmask(func, 16, 0xffff)) {
	}

	constexpr static Gate interrupt_gate(void (*func)()) {
		return Gate{func, GateType::interrupt};
	}

	[[nodiscard]] GateType type() const { return _gate_kind; }

private:
#define reserved macro_concat(_reserved_, __COUNTER__)
	uint16_t offset_00_15_;
	uint16_t _segment_selector = KERNEL_CODE_SELECTOR;
	uint8_t	reserved: 5 = 0;
	uint8_t reserved: 3 = 0;
	GateType _gate_kind: 3;
	OperationSize _db: 1 = OperationSize::db32_bit;
	uint8_t _zero_bit: 1 = 0;
	ProtectionRing _dpl: 2 = ProtectionRing::kernel;
	Present	_present: 1 = Present::present;
	uint16_t offset_16_31_;
#undef reserved
};


}
