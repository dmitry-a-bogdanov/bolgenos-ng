#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

#include "asm.hpp"
#include "mmu.hpp"
#include "mem_utils.hpp"

#include <lib/forward_list.hpp>


namespace lib {
	class ostream;
}

namespace irq {


/// \brief IRQ line type.
///
/// Type for holding IRQ lines. Alias to unsigned one-byte type.
using irq_t = uint8_t;


/// Processor exception type.
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

/// Stack pointer type.
using stack_ptr_t = void *;


class IRQHandler {
public:
	enum class status_t {
		NONE,
		HANDLED,
	};

	virtual ~IRQHandler()
	{
	}


	virtual status_t handle_irq(irq_t vector) = 0;
};


class ExceptionHandler {
public:
	virtual ~ExceptionHandler()
	{
	}

	virtual void handle_exception(stack_ptr_t frame) = 0;
};



/// \brief Last IRQ line.
///
/// Last IRQ line that can be configured configured and supported by xxPIC.
constexpr size_t LAST_LINE = 0xff;


/// \brief Size of IRQ gate.
///
/// IRQ gate must be 8 bytes on x86 arch.
constexpr size_t GATE_SIZE = 8;


/// \brief Total number of IRQ lines.
///
/// Total number of interrupts that are configured and supported by xxPIC.
constexpr size_t NUMBER_OF_LINES = LAST_LINE + 1;



class InterruptsManager {
public:
	void add_handler(irq_t vector, IRQHandler *handler);
	void add_handler(exception_t exception, ExceptionHandler *handler);

	static InterruptsManager *instance();
protected:
	InterruptsManager();

	IRQHandler::status_t dispatch_interrupt(irq_t vector);
	IRQHandler::status_t dispatch_exception(exception_t exception, stack_ptr_t frame_pointer);

	__attribute__((regparm(0),cdecl)) static void handle_irq(irq_t vector, void *frame);
	static bool is_exception(irq_t vector);
private:
	lib::forward_list<IRQHandler *> _irq_handlers[NUMBER_OF_LINES];
	lib::forward_list<ExceptionHandler *> _exceptions_handlers[exception_t::max];
	_irq_aligned_ table_pointer idt_pointer{};

	static InterruptsManager *_instance;
};



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


/// \brief Registers dump on stack.
///
/// The structure aliases stack state after calling `pushal`
/// assembler instruction.
struct __attribute__((packed)) registers_dump_t {
	/// Content of EDI register.
	uint32_t	edi;
	/// Content of ESI register.
	uint32_t	esi;
	/// Content of EBP register.
	void*		ebp;
	/// Content of ESP register.
	void*		esp;
	/// Content of EBX register.
	uint32_t	ebx;
	/// Content of EDX register.
	uint32_t	edx;
	/// Content of ECX register.
	uint32_t	ecx;
	/// Content of EAX register.
	uint32_t	eax;
};


/// \brief Execution info.
///
/// The structure aliases data that is stored by CPU when doing context switch
/// to interrupt or exception handler.
struct __attribute__((packed)) execution_info_dump_t {
	/// Instruction pointer.
	void*		eip;
	/// Code segment.
	uint16_t	cs;
	/// Content of EFLAGS register.
	uint32_t	eflags;
};


/// \brief Interrupt frame with error code.
///
/// The structure aliases stack frame after switching to exceptions that stores
/// error code on stack.
struct __attribute__((packed)) int_frame_error_t {
	/// Dump of registers.
	registers_dump_t regs;
	/// Error code.
	uint32_t error_code;
	/// Execution info.
	execution_info_dump_t exe;
};


/// \brief Interrupt frame without error code.
///
/// The structure aliases stack frame after switching to exceptions that doesn't
/// store error code on stack.
struct __attribute__((packed)) int_frame_noerror_t {
	/// Dump of registers.
	registers_dump_t regs;
	/// Execution info.
	execution_info_dump_t exe;
};


// output functions
lib::ostream& operator <<(lib::ostream&, const registers_dump_t&);
lib::ostream& operator <<(lib::ostream&, const execution_info_dump_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_error_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_noerror_t&);


} // namespace irq





namespace bolgenos { namespace irq {


using irq_t = uint8_t;


/// Type of pointer to IRQ dispatcher function.
using dispatcher_function_t =  __attribute__((regparm(0),cdecl))  void (irq_t vector, ::irq::stack_ptr_t frame);

void set_dispatcher(dispatcher_function_t *f);
dispatcher_function_t* get_dispatcher();




struct __attribute__((packed)) gate_t {
	using func_type = void (*)();
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
			segment_(mmu::KERNEL_CODE_SEGMENT_POINTER),
			reserved_(0),
			zeros_(0),
			gate_kind_(gate_kind_type::interrupt),
			flag_32_bit_(bitness_flag_type::bits_32),
			zero_bit_(0),
			dpl_(protection_ring_t::ring_null),
			present_(presence_type::presence_yes),
			offset_16_31_(0x0) {
	}


	explicit constexpr gate_t(std::uintptr_t func,
			gate_kind_type kind = gate_kind_type::trap)
			: offset_00_15_(bitmask(func, 0, std::uintptr_t{ 0xffff })),
			segment_(mmu::KERNEL_CODE_SEGMENT_POINTER),
			reserved_(0),
			zeros_(0),
			gate_kind_(kind),
			flag_32_bit_(bitness_flag_type::bits_32),
			zero_bit_(0),
			dpl_(protection_ring_t::ring_kernel),
			present_(presence_type::presence_yes),
			offset_16_31_(bitmask(func, 16, std::uintptr_t{ 0xffff })) {
	}


	static gate_t interrupt_gate(func_type func) {
		auto func_addr = reinterpret_cast<std::uintptr_t>(func);
		gate_t gate(func_addr, gate_kind_type::interrupt);
		return gate;
	}


	static gate_t trap_gate(func_type func) {
		auto func_addr = reinterpret_cast<std::uintptr_t>(func);
		gate_t gate(func_addr, gate_kind_type::trap);
		return gate;
	}
private:
#ifdef __GNUG__
	std::uintptr_t		offset_00_15_	:16;
	segment_type		segment_;
	nothing_type		reserved_	:5;
	nothing_type		zeros_		:3;
	gate_kind_type		gate_kind_	:3;
	bitness_flag_type 	flag_32_bit_	:1;
	nothing_type		zero_bit_	:1;
	protection_ring_t	dpl_		:2;
	presence_type		present_	:1;
	std::uintptr_t		offset_16_31_	:16;
#else
#error Only GCC is supported
#endif
};

static_assert(sizeof(gate_t) == ::irq::GATE_SIZE, "gate_t has wrong size");


} }


