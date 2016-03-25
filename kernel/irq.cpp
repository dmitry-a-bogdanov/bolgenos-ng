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

namespace {


struct __attribute__((packed)) trap_frame_type {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t eflags;
	uint16_t cs;
	uint32_t eip;
	uint32_t err_code;
};


cio::OutStream& operator << (cio::OutStream &stream,
		const trap_frame_type &frame) {
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


/// Type list of Interrupt Service Routines.
using handlers_list_t = lib::list<irq::irq_handler_t>;


/// Array of lists of Interrupt Service Routines.
handlers_list_t registered_isrs[irq::lines_number::value];


void irq_dispatcher(irq::irq_t vector, trap_frame_type *frame);


irq::irq_return_t do_breakpoint(irq::irq_t, irq::stack_pointer_type stack_ptr) {
	auto trap_frame = reinterpret_cast<trap_frame_type *>(stack_ptr);
	cio::cnotice << "breakpoint has been caught: " << cio::endl;
	cio::cnotice << *trap_frame << cio::endl;
	return irq::irq_return_t::handled;
}


using gate_field_t = uint32_t;


struct __attribute__((packed)) gate_t {
	using asm_function_type = void (*)();
	using asm_function_ptr_type = uint32_t;
	using segment_id_t = uint16_t;
	using reserved_type = uint32_t;


	enum irq_gate_type {
		task = 0x5,
		intr = 0x6,
		trap = 0x7,
	};


	enum flag_32_bit_type {
		bits_32 = 1,
		bits_16 = 0,
	};


	enum zero_flag_type {
		just_zero = 0x0
	};


	enum present_type {
		present_unknown = 0x0,
		not_present = 0x0,
		present = 0x1,
	};


	constexpr gate_t()
			: offset_00_15_(bitmask<asm_function_ptr_type>(nullptr)),
			segment_(mmu::kernel_cs_ptr::value),
			_reserved(0),
			zeros(0),
			gate_type(irq_gate_type::intr),
			flag_32_bit(flag_32_bit_type::bits_32),
			zero_bit_(zero_flag_type::just_zero),
			dpl_(protection_ring_t::dpl_null),
			present_(present_type::present),
			offset_16_31_(bitmask<asm_function_ptr_type>(nullptr)) {
	}

	explicit constexpr gate_t(asm_function_ptr_type offs)
			: offset_00_15_(bitmask(offs, 0, 0xffff)),
			segment_(mmu::kernel_cs_ptr::value),
			_reserved(0),
			zeros(0),
			gate_type(irq_gate_type::intr),
			flag_32_bit(flag_32_bit_type::bits_32),
			zero_bit_(zero_flag_type::just_zero),
			dpl_(protection_ring_t::kernel),
			present_(present_type::present),
			offset_16_31_(bitmask(offs, 16, 0xffff)) {
	}

	static gate_t make_int_gate(asm_function_type _f) {
		auto f = reinterpret_cast<asm_function_ptr_type>(_f);
		gate_t gate;
		gate.offset_00_15_ = bitmask(f, 0, 0xffff);
		gate.offset_16_31_ = bitmask(f, 16, 0xffff);
		gate.segment_ = mmu::kernel_cs_ptr::value;
		return gate;
	}
private:
	asm_function_ptr_type offset_00_15_:16;
	segment_id_t segment_;
	reserved_type _reserved:5;
	gate_field_t zeros:3;
	irq_gate_type gate_type:3;
	flag_32_bit_type flag_32_bit:1;
	zero_flag_type zero_bit_:1;
	protection_ring_t dpl_:2;
	present_type present_:1;
	asm_function_ptr_type offset_16_31_:16;
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
	void IRQ_STAGE_C(num)(trap_frame_type *frame) {			\
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
		tbl[num] = gate_t::make_int_gate(IRQ_STAGE_ASM(num));	\
	} while(0)


DECL_IRQ_HANDLER(0, irq_dispatcher);
DECL_IRQ_HANDLER(1, irq_dispatcher);
DECL_IRQ_HANDLER(2, irq_dispatcher);
DECL_IRQ_HANDLER(3, irq_dispatcher);
DECL_IRQ_HANDLER(4, irq_dispatcher);
DECL_IRQ_HANDLER(5, irq_dispatcher);
DECL_IRQ_HANDLER(6, irq_dispatcher);
DECL_IRQ_HANDLER(7, irq_dispatcher);
DECL_IRQ_HANDLER(8, irq_dispatcher);
DECL_IRQ_HANDLER(9, irq_dispatcher);
DECL_IRQ_HANDLER(10, irq_dispatcher);
DECL_IRQ_HANDLER(11, irq_dispatcher);
DECL_IRQ_HANDLER(12, irq_dispatcher);
DECL_IRQ_HANDLER(13, irq_dispatcher);
DECL_IRQ_HANDLER(14, irq_dispatcher);
DECL_IRQ_HANDLER(15, irq_dispatcher);
DECL_IRQ_HANDLER(16, irq_dispatcher);
DECL_IRQ_HANDLER(17, irq_dispatcher);
DECL_IRQ_HANDLER(18, irq_dispatcher);
DECL_IRQ_HANDLER(19, irq_dispatcher);
DECL_IRQ_HANDLER(20, irq_dispatcher);
DECL_IRQ_HANDLER(21, irq_dispatcher);
DECL_IRQ_HANDLER(22, irq_dispatcher);
DECL_IRQ_HANDLER(23, irq_dispatcher);
DECL_IRQ_HANDLER(24, irq_dispatcher);
DECL_IRQ_HANDLER(25, irq_dispatcher);
DECL_IRQ_HANDLER(26, irq_dispatcher);
DECL_IRQ_HANDLER(27, irq_dispatcher);
DECL_IRQ_HANDLER(28, irq_dispatcher);
DECL_IRQ_HANDLER(29, irq_dispatcher);
DECL_IRQ_HANDLER(30, irq_dispatcher);
DECL_IRQ_HANDLER(31, irq_dispatcher);
DECL_IRQ_HANDLER(32, irq_dispatcher);
DECL_IRQ_HANDLER(33, irq_dispatcher);
DECL_IRQ_HANDLER(34, irq_dispatcher);
DECL_IRQ_HANDLER(35, irq_dispatcher);
DECL_IRQ_HANDLER(36, irq_dispatcher);
DECL_IRQ_HANDLER(37, irq_dispatcher);
DECL_IRQ_HANDLER(38, irq_dispatcher);
DECL_IRQ_HANDLER(39, irq_dispatcher);
DECL_IRQ_HANDLER(40, irq_dispatcher);
DECL_IRQ_HANDLER(41, irq_dispatcher);
DECL_IRQ_HANDLER(42, irq_dispatcher);
DECL_IRQ_HANDLER(43, irq_dispatcher);
DECL_IRQ_HANDLER(44, irq_dispatcher);
DECL_IRQ_HANDLER(45, irq_dispatcher);
DECL_IRQ_HANDLER(46, irq_dispatcher);
DECL_IRQ_HANDLER(47, irq_dispatcher);


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

	register_handler(3, do_breakpoint);
}


void irq::register_handler(irq_t vector, irq_handler_t routine) {
	if (!registered_isrs[vector].push_front(routine)) {
		panic("Failed to register ISR");
	}
}

namespace {


void default_isr(irq::irq_t vector) {
	cio::ccrit << "Unhandled IRQ" << vector << cio::endl;
	panic("Fatal interrupt");
}


void irq_dispatcher(irq::irq_t vector, trap_frame_type *frame) {
	auto& handlers = registered_isrs[vector];
	auto used_handler = lib::find_if(handlers.begin(), handlers.end(),
		[vector, frame] (const irq::irq_handler_t &handler) -> bool {
			return handler(vector, reinterpret_cast<
					irq::stack_pointer_type>(frame)) ==
							irq::irq_return_t::handled;
	});
	if (used_handler == handlers.end()) {
		default_isr(vector);
	}
}



} // namespace
