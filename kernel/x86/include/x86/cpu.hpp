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

	[[gnu::noinline]]
	static void switch_task_to(uint16_t segment_selector);
private:
	GDT _gdt{};
};

}
