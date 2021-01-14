#pragma once

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.hpp>

#include <array.hpp>

namespace x86 {

struct __attribute__((packed)) gate_t {
	using func_type = void (*)();
	using func_addr_type = uint32_t;
	using segment_type = uint16_t;
	using nothing_type = uint32_t;

	static constexpr func_addr_type get_addr(func_type fp) {
		return (func_addr_type) fp;
	}

	enum gate_kind_type {
		task		= 0x5,
		interrupt	= 0x6,
		trap		= 0x7,
	};


	enum bitness_flag_type {
		bits_32		= 1,
		bits_16		= 0,
	};


	enum presence_type {
		presence_none	= 0x0,
		presence_yes	= 0x1,
	};


	constexpr gate_t()
		: offset_00_15_(0x0),
		  segment_(x86::KERNEL_CODE_SEGMENT_SELECTOR),
		  reserved_(0),
		  zeros_(0),
		  gate_kind_(gate_kind_type::interrupt),
		  flag_32_bit_(bitness_flag_type::bits_32),
		  zero_bit_(0),
		  dpl_(ProtectionRing::null),
		  present_(presence_type::presence_yes),
		  offset_16_31_(0x0) {
	}


	explicit constexpr gate_t(func_addr_type func,
				  gate_kind_type kind = gate_kind_type::trap)
		: offset_00_15_(bitmask(func, 0, 0xffff)),
		  segment_(x86::KERNEL_CODE_SEGMENT_SELECTOR),
		  reserved_(0),
		  zeros_(0),
		  gate_kind_(kind),
		  flag_32_bit_(bitness_flag_type::bits_32),
		  zero_bit_(0),
		  dpl_(ProtectionRing::kernel),
		  present_(presence_type::presence_yes),
		  offset_16_31_(bitmask(func, 16, 0xffff)) {
	}


	constexpr static gate_t interrupt_gate(func_type func) {
		const auto func_addr = get_addr(func);
		gate_t gate(func_addr, gate_kind_type::interrupt);
		return gate;
	}


	static gate_t trap_gate(func_type func) {
		auto func_addr = reinterpret_cast<func_addr_type>(func);
		gate_t gate(func_addr, gate_kind_type::trap);
		return gate;
	}
//private:
	func_addr_type		offset_00_15_	:16;
	segment_type		segment_;
	nothing_type		reserved_	:5;
	nothing_type		zeros_		:3;
	gate_kind_type		gate_kind_	:3;
	bitness_flag_type 	flag_32_bit_	:1;
	nothing_type		zero_bit_	:1;
	ProtectionRing	dpl_		:2;
	presence_type		present_	:1;
	func_addr_type		offset_16_31_	:16;
};

class IDT {
public:

	/// Type of pointer to IRQ dispatcher function.
	using irq_dispatcher_func_t =  void (*)(irq::irq_t vector, irq::stack_ptr_t frame);
	IDT();
	void reload_table(irq_dispatcher_func_t irq_dispatcher);
	static irq_dispatcher_func_t _dispatcher;
private:
	alignas(cpu_alignment) lib::Array<gate_t, irq::NUMBER_OF_LINES> _idt;
	alignas(cpu_alignment) table_pointer _idt_pointer;
};

lib::ostream& operator<<(lib::ostream& out, const gate_t& gate);

} // namespace x86
