#pragma once

#include <lib/type_traits.hpp>

#include "asm.h"
#include "mmu.hpp"
#include "mem_utils.hpp"
#include "stdtypes.hpp"


namespace lib {
	class ostream;
}


namespace irq {


/// \brief IRQ line type.
///
/// Type for holding IRQ lines. Alias to unsigned one-byte type.
using irq_t = uint8_t;


enum exception_t: irq_t {
	divide_by_zero			= 0,
	debug_exception			= 1,
	breakpoint			= 3,
	overflow_exception		= 4,
	bound_range_exceeded		= 5,
	invalid_opcode			= 6,
	device_not_available		= 7,
	double_fault,
	// coprocessor segment overrun
	invalid_tss			= 10,
	segment_not_present		= 11,
	stack_segment_fault		= 12,
	general_protection_fault	= 13,
	page_fault			= 14,
	// reserved
	x87_fp_exception		= 16,
	alignment_error			= 17,
	machine_check_error		= 18,
	simd_fp_exception		= 19,
	virtualization_exception	= 20,
	// reserved 21-29
	security_exception		= 30,
	// reserved
	max = 0x20,
};


using stack_ptr_t = void *;


enum class irq_return_t {
	none,
	handled,
};


/// \brief IRQ handler function type.
///
/// Type for holding IRQ handler routine. Function type accepts IRQ line as
/// parameter and returns nothing.
using irq_handler_t = irq_return_t (*)(irq_t);


/// \brief Last IRQ line.
///
/// Last IRQ line that can be configured configured and supported by xxPIC.
using last_line = lib::integral_constant<size_t, 0x2f>;


/// \brief Total number of IRQ lines.
///
/// Total number of interrupts that are configured and supported by xxPIC.
using lines_number = lib::integral_constant<size_t, last_line::value + 1>;


/// \brief Enable interrupts.
///
/// Enable interrupts by setting Interrupt Flag for CPU.
inline void enable() {
	asm volatile ("sti\n");
}


/// \brief Disable interrupts.
///
/// Disable interrupts by clearing Interrupt Flag for CPU.
///
inline void disable() {
	asm volatile ("cli\n");
}


/// \brief Initialize IRQ subsystem.
///
/// Function registers generic Interrupt Service Routine and loads
/// Interrupt Descriptor Table.
void init();


/// \brief Register interrupt handler function.
///
/// Register specified function to be called when given IRQ happens.
///
/// \param vector IRQ line that will use provided function.
/// \param routine Function to be called when specified IRQ vector happens.
void register_irq_handler(irq_t vector, irq_handler_t routine);


struct __attribute__((packed)) registers_dump_t {
	uint32_t	edi;
	uint32_t	esi;
	void*		ebp;
	void*		esp;
	uint32_t	ebx;
	uint32_t	edx;
	uint32_t	ecx;
	uint32_t	eax;
};


struct __attribute__((packed)) execution_info_dump_t {
	void*		eip;
	uint16_t	cs;
	uint32_t	eflags;
};


struct __attribute__((packed)) int_frame_error_t {
	registers_dump_t regs;
	uint32_t error_code;
	execution_info_dump_t exe;
};


struct __attribute__((packed)) int_frame_noerror_t {
	registers_dump_t regs;
	execution_info_dump_t exe;
};


lib::ostream& operator <<(lib::ostream&, const registers_dump_t&);
lib::ostream& operator <<(lib::ostream&, const execution_info_dump_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_error_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_noerror_t&);


using exc_handler_t = void (*)(void *frame);


void register_exc_handler(exception_t exception, exc_handler_t handler);


struct __attribute__((packed)) gate_t {
	using func_type = void (*)();
	using func_addr_type = uint32_t;
	using segment_type = uint16_t;
	using nothing_type = uint32_t;


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
			segment_(mmu::kernel_cs_ptr::value),
			reserved_(0),
			zeros_(0),
			gate_kind_(gate_kind_type::interrupt),
			flag_32_bit_(bitness_flag_type::bits_32),
			zero_bit_(0),
			dpl_(protection_ring_t::ring_null),
			present_(presence_type::presence_yes),
			offset_16_31_(0x0) {
	}


	explicit constexpr gate_t(func_addr_type func,
			gate_kind_type kind = gate_kind_type::trap)
			: offset_00_15_(bitmask(func, 0, 0xffff)),
			segment_(mmu::kernel_cs_ptr::value),
			reserved_(0),
			zeros_(0),
			gate_kind_(kind),
			flag_32_bit_(bitness_flag_type::bits_32),
			zero_bit_(0),
			dpl_(protection_ring_t::ring_kernel),
			present_(presence_type::presence_yes),
			offset_16_31_(bitmask(func, 16, 0xffff)) {
	}


	static gate_t interrupt_gate(func_type func) {
		auto func_addr = reinterpret_cast<func_addr_type>(func);
		gate_t gate(func_addr, gate_kind_type::interrupt);
		return gate;
	}


	static gate_t trap_gate(func_type func) {
		auto func_addr = reinterpret_cast<func_addr_type>(func);
		gate_t gate(func_addr, gate_kind_type::trap);
		return gate;
	}
private:
	func_addr_type		offset_00_15_	:16;
	segment_type		segment_;
	nothing_type		reserved_	:5;
	nothing_type		zeros_		:3;
	gate_kind_type		gate_kind_	:3;
	bitness_flag_type 	flag_32_bit_	:1;
	nothing_type		zero_bit_	:1;
	protection_ring_t	dpl_		:2;
	presence_type		present_	:1;
	func_addr_type		offset_16_31_	:16;
};


extern "C" void irq_dispatcher(irq::irq_t vector, void* frame);


} // namespace irq
