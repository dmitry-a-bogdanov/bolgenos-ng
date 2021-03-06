#pragma once

#include "gdt.hpp"
#include "idt.hpp"
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

	constexpr IDT& idt() noexcept {
		return _idt;
	}

	void load_kernel_segments();
	void load_interrupts_table();

	[[gnu::noinline]]
	static void switch_task_to(uint16_t segment_selector);
private:
	GDT _gdt{};
	IDT _idt{};
};

}
