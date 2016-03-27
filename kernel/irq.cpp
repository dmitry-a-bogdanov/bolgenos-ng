#include <bolgenos-ng/irq.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/error.h>

#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/pic_common.hpp>
#include <bolgenos-ng/stdtypes.hpp>

#include <lib/algorithm.hpp>
#include <lib/list.hpp>


cio::OutStream& irq::operator << (cio::OutStream &stream,
		const trap_frame_t &frame) {
	stream	<< " EFLAGS = " << frame.eflags
		<< " CS = " << frame.cs
		<< " EIP = " << frame.eip
		<< cio::endl;
	stream	<< " ESP = " << frame.esp
		<< " EBP = " << frame.ebp
		<< cio::endl;
	stream	<< " EAX = " << frame.eax
		<< " EBX = " << frame.ebx
		<< " ECX = " << frame.ecx
		<< " EDX = " << frame.edx;
	return stream;
}

namespace {






using exc_handlers_list_t = lib::list<irq::exc_handler_t>;

/// Type list of Interrupt Service Routines.
using irq_handlers_list_t = lib::list<irq::irq_handler_t>;


/// Array of lists of Interrupt Service Routines.
irq_handlers_list_t irq_handlers[irq::lines_number::value];\


exc_handlers_list_t exc_handlers[static_cast<int>(irq::exception_t::max)];
//exc_handlers_list_t exc_handlers[irq::exception_t::max];

void dispatcher(irq::irq_t vector, irq::trap_frame_t *frame);


void do_breakpoint(irq::trap_frame_t *frame) {
	cio::cnotice << "breakpoint has been caught: " << cio::endl;
	cio::cnotice << *frame << cio::endl;
}


using gate_field_t = uint32_t;


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
			: offset_00_15(0x0),
			segment(mmu::kernel_cs_ptr::value),
			_reserved(0),
			_zeros(0),
			gate_kind(gate_kind_type::interrupt),
			flag_32_bit(bitness_flag_type::bits_32),
			_zero_bit(0),
			dpl(protection_ring_t::ring_null),
			present(presence_type::presence_yes),
			offset_16_31(0x0) {
	}


	explicit constexpr gate_t(func_addr_type func,
			gate_kind_type kind = gate_kind_type::trap)
			: offset_00_15(bitmask(func, 0, 0xffff)),
			segment(mmu::kernel_cs_ptr::value),
			_reserved(0),
			_zeros(0),
			gate_kind(kind),
			flag_32_bit(bitness_flag_type::bits_32),
			_zero_bit(0),
			dpl(protection_ring_t::kernel),
			present(presence_type::presence_yes),
			offset_16_31(bitmask(func, 16, 0xffff)) {
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
	func_addr_type		offset_00_15	:16;
	segment_type		segment;
	nothing_type		_reserved	:5;
	nothing_type		_zeros		:3;
	gate_kind_type		gate_kind	:3;
	bitness_flag_type 	flag_32_bit	:1;
	nothing_type		_zero_bit	:1;
	protection_ring_t	dpl		:2;
	presence_type		present		:1;
	func_addr_type		offset_16_31	:16;
};
static_assert(sizeof(gate_t) == 8, "gate_t has wrong size");


/// Get name of asm-part of IRQ handler.
///
/// \param num number of IRQ.
#define IRQ_STAGE_ASM(num)						\
	_irq_stage_asm_ ## num


/// Get name of C-part of IRQ handler.
///
/// \param num number of IRQ.
#define IRQ_STAGE_C(num)						\
	_irq_stage_c_ ## num


/// Declare asm-part IRQ handler.
///
/// \param num number of IRQ.
#define DECL_IRQ_HANDLER_ASM(num)					\
	asm(								\
		".align 16\n"						\
			stringify(IRQ_STAGE_ASM(num)) ":\n"		\
			"pushal\n"					\
			"push %esp\n"					\
			"call " stringify(IRQ_STAGE_C(num)) "\n"	\
			"pop %esp\n"					\
			"popal\n"					\
			"iret\n"					\
	);								\
	extern "C" void IRQ_STAGE_ASM(num)()


/// Declare C-part of IRQ handler.
///
/// \param num number of IRQ.
/// \param function function to be called on IRQ.
#define DECL_IRQ_HANDLER_C(num, function)				\
	extern "C" __attribute__((used, regparm(0)))			\
	void IRQ_STAGE_C(num)(irq::trap_frame_t *frame) {			\
		function(num, frame);					\
		pic::end_of_irq(num);					\
	}


/// Declare two-stage IRQ handler.
///
/// \param num number of IRQ.
/// \param function function to be called on IRQ.
#define DECL_IRQ_HANDLER(num, function)					\
	DECL_IRQ_HANDLER_ASM(num);					\
	DECL_IRQ_HANDLER_C(num, function)


/// Write gate for standard 2-stage IRQ handler to table.
///
/// \param num number of IRQ.
/// \param tbl interrupt descriptor table.
#define WRITE_GENERIC_GATE(num, tbl)					\
	do {								\
		tbl[num] = gate_t::interrupt_gate(IRQ_STAGE_ASM(num));	\
	} while(0)


DECL_IRQ_HANDLER(0, dispatcher);
DECL_IRQ_HANDLER(1, dispatcher);
DECL_IRQ_HANDLER(2, dispatcher);
DECL_IRQ_HANDLER(3, dispatcher);
DECL_IRQ_HANDLER(4, dispatcher);
DECL_IRQ_HANDLER(5, dispatcher);
DECL_IRQ_HANDLER(6, dispatcher);
DECL_IRQ_HANDLER(7, dispatcher);
DECL_IRQ_HANDLER(8, dispatcher);
DECL_IRQ_HANDLER(9, dispatcher);
DECL_IRQ_HANDLER(10, dispatcher);
DECL_IRQ_HANDLER(11, dispatcher);
DECL_IRQ_HANDLER(12, dispatcher);
DECL_IRQ_HANDLER(13, dispatcher);
DECL_IRQ_HANDLER(14, dispatcher);
DECL_IRQ_HANDLER(15, dispatcher);
DECL_IRQ_HANDLER(16, dispatcher);
DECL_IRQ_HANDLER(17, dispatcher);
DECL_IRQ_HANDLER(18, dispatcher);
DECL_IRQ_HANDLER(19, dispatcher);
DECL_IRQ_HANDLER(20, dispatcher);
DECL_IRQ_HANDLER(21, dispatcher);
DECL_IRQ_HANDLER(22, dispatcher);
DECL_IRQ_HANDLER(23, dispatcher);
DECL_IRQ_HANDLER(24, dispatcher);
DECL_IRQ_HANDLER(25, dispatcher);
DECL_IRQ_HANDLER(26, dispatcher);
DECL_IRQ_HANDLER(27, dispatcher);
DECL_IRQ_HANDLER(28, dispatcher);
DECL_IRQ_HANDLER(29, dispatcher);
DECL_IRQ_HANDLER(30, dispatcher);
DECL_IRQ_HANDLER(31, dispatcher);
DECL_IRQ_HANDLER(32, dispatcher);
DECL_IRQ_HANDLER(33, dispatcher);
DECL_IRQ_HANDLER(34, dispatcher);
DECL_IRQ_HANDLER(35, dispatcher);
DECL_IRQ_HANDLER(36, dispatcher);
DECL_IRQ_HANDLER(37, dispatcher);
DECL_IRQ_HANDLER(38, dispatcher);
DECL_IRQ_HANDLER(39, dispatcher);
DECL_IRQ_HANDLER(40, dispatcher);
DECL_IRQ_HANDLER(41, dispatcher);
DECL_IRQ_HANDLER(42, dispatcher);
DECL_IRQ_HANDLER(43, dispatcher);
DECL_IRQ_HANDLER(44, dispatcher);
DECL_IRQ_HANDLER(45, dispatcher);
DECL_IRQ_HANDLER(46, dispatcher);
DECL_IRQ_HANDLER(47, dispatcher);


gate_t idt[irq::lines_number::value] _irq_aligned_;
table_pointer idt_pointer _irq_aligned_ = {sizeof(idt) - 1,
		static_cast<void *>(&idt)};

} // namespace

void irq::init() {
	// See comment__why_not_use_counter
	WRITE_GENERIC_GATE(0, idt);
	WRITE_GENERIC_GATE(1, idt);
	WRITE_GENERIC_GATE(2, idt);
	WRITE_GENERIC_GATE(3, idt);
	WRITE_GENERIC_GATE(4, idt);
	WRITE_GENERIC_GATE(5, idt);
	WRITE_GENERIC_GATE(6, idt);
	WRITE_GENERIC_GATE(7, idt);
	WRITE_GENERIC_GATE(8, idt);
	WRITE_GENERIC_GATE(9, idt);
	WRITE_GENERIC_GATE(10, idt);
	WRITE_GENERIC_GATE(11, idt);
	WRITE_GENERIC_GATE(12, idt);
	WRITE_GENERIC_GATE(13, idt);
	WRITE_GENERIC_GATE(14, idt);
	WRITE_GENERIC_GATE(15, idt);
	WRITE_GENERIC_GATE(16, idt);
	WRITE_GENERIC_GATE(17, idt);
	WRITE_GENERIC_GATE(18, idt);
	WRITE_GENERIC_GATE(19, idt);
	WRITE_GENERIC_GATE(20, idt);
	WRITE_GENERIC_GATE(21, idt);
	WRITE_GENERIC_GATE(22, idt);
	WRITE_GENERIC_GATE(23, idt);
	WRITE_GENERIC_GATE(24, idt);
	WRITE_GENERIC_GATE(25, idt);
	WRITE_GENERIC_GATE(26, idt);
	WRITE_GENERIC_GATE(27, idt);
	WRITE_GENERIC_GATE(28, idt);
	WRITE_GENERIC_GATE(29, idt);
	WRITE_GENERIC_GATE(30, idt);
	WRITE_GENERIC_GATE(31, idt);
	WRITE_GENERIC_GATE(32, idt);
	WRITE_GENERIC_GATE(33, idt);
	WRITE_GENERIC_GATE(34, idt);
	WRITE_GENERIC_GATE(35, idt);
	WRITE_GENERIC_GATE(36, idt);
	WRITE_GENERIC_GATE(37, idt);
	WRITE_GENERIC_GATE(38, idt);
	WRITE_GENERIC_GATE(39, idt);
	WRITE_GENERIC_GATE(40, idt);
	WRITE_GENERIC_GATE(41, idt);
	WRITE_GENERIC_GATE(42, idt);
	WRITE_GENERIC_GATE(43, idt);
	WRITE_GENERIC_GATE(44, idt);
	WRITE_GENERIC_GATE(45, idt);
	WRITE_GENERIC_GATE(46, idt);
	WRITE_GENERIC_GATE(47, idt);

	asm volatile("lidt %0"::"m" (idt_pointer));

	register_exc_handler(irq::exception_t::breakpoint, do_breakpoint);
}


void irq::register_irq_handler(irq_t vector, irq_handler_t routine) {
	if (!irq_handlers[vector].push_front(routine)) {
		panic("failed to register interrupt handler");
	}
}


void irq::register_exc_handler(exception_t exception, exc_handler_t routine) {
	if (!exc_handlers[static_cast<int>(exception)].push_front(routine)) {
		panic("failed to register exception handler");
	}
}


namespace {


void default_handler(irq::irq_t vector) {
	cio::ccrit << "Unhandled IRQ" << vector << cio::endl;
	panic("Fatal interrupt");
}


irq::irq_return_t exception_dispatcher(irq::irq_t exception,
		irq::trap_frame_t *frame) {
	auto& handlers = exc_handlers[exception];

	if (handlers.empty()) {
		return irq::irq_return_t::none;
	}

	lib::for_each(handlers.begin(), handlers.end(),
		[frame](const irq::exc_handler_t& handler) -> void {
			handler(frame);
	});

	return irq::irq_return_t::handled;
}


irq::irq_return_t interrupts_dispatcher(irq::irq_t vector) {
	auto& handlers = irq_handlers[vector];
	auto used_handler = lib::find_if(handlers.begin(), handlers.end(),
		[vector] (const irq::irq_handler_t &handler) -> bool {
			return handler(vector) == irq::irq_return_t::handled;
	});
	if (used_handler == handlers.end()) {
		return irq::irq_return_t::none;
	}
	return irq::irq_return_t::handled;
}


void dispatcher(irq::irq_t vector, irq::trap_frame_t *frame) {
	irq::irq_return_t status;

	if (vector < irq::exception_t::max) {
		status = exception_dispatcher(vector, frame);
	} else {
		status = interrupts_dispatcher(vector);
	}

	if (status != irq::irq_return_t::handled) {
		default_handler(vector);
	}
}



} // namespace
