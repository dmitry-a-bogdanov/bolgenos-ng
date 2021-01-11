#pragma once

#include "gdt.hpp"
#include "eflags.hpp"

namespace x86 {

class Processor {
public:

	static EFlags flags();

	[[nodiscard]] constexpr const GDT& gdt() const noexcept {
		return _gdt;
	}

	constexpr GDT& gdt() noexcept {
		return _gdt;
	}

	void load_kernel_segments();
private:
	GDT _gdt{};
};

}
