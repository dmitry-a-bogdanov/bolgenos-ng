#include <bolgenos-ng/irq.hpp>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.h>

#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/pic_common.hpp>
#include <bolgenos-ng/stdtypes.hpp>

#include <lib/algorithm.hpp>
#include <lib/list.hpp>

namespace {


using handlers_list_t = lib::list<irq::irq_handler_t>;

handlers_list_t registered_isrs[irq::lines_number::value];


void irq_dispatcher(irq::irq_t vector);


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


#define __decl_isr_stage_asm(num)					\
	asm(								\
		".align 16\n"						\
		"__isr_stage_asm_" #num ":\n"				\
			"pushal\n"					\
			"call __isr_stage_c_" #num "\n"			\
			"popal\n"					\
			"iret\n"					\
	);								\
	extern "C" void __isr_stage_asm_ ## num()


#define __decl_isr_stage_c(num, generic_isr)				\
	extern "C" void __attribute__((used)) __isr_stage_c_ ## num () {\
		generic_isr(num);					\
		pic::end_of_irq(num);					\
	}


#define __decl_isr(num, function)					\
	__decl_isr_stage_asm(num);					\
	__decl_isr_stage_c(num, function)


#define __decl_common_gate(num, table)					\
	do {								\
		gate_t gate = gate_t::make_int_gate(__isr_stage_asm_ ## num);	\
		table[num] = gate;			\
	} while(0)


__decl_isr(0, irq_dispatcher);
__decl_isr(1, irq_dispatcher);
__decl_isr(2, irq_dispatcher);
__decl_isr(3, irq_dispatcher);
__decl_isr(4, irq_dispatcher);
__decl_isr(5, irq_dispatcher);
__decl_isr(6, irq_dispatcher);
__decl_isr(7, irq_dispatcher);
__decl_isr(8, irq_dispatcher);
__decl_isr(9, irq_dispatcher);
__decl_isr(10, irq_dispatcher);
__decl_isr(11, irq_dispatcher);
__decl_isr(12, irq_dispatcher);
__decl_isr(13, irq_dispatcher);
__decl_isr(14, irq_dispatcher);
__decl_isr(15, irq_dispatcher);
__decl_isr(16, irq_dispatcher);
__decl_isr(17, irq_dispatcher);
__decl_isr(18, irq_dispatcher);
__decl_isr(19, irq_dispatcher);
__decl_isr(20, irq_dispatcher);
__decl_isr(21, irq_dispatcher);
__decl_isr(22, irq_dispatcher);
__decl_isr(23, irq_dispatcher);
__decl_isr(24, irq_dispatcher);
__decl_isr(25, irq_dispatcher);
__decl_isr(26, irq_dispatcher);
__decl_isr(27, irq_dispatcher);
__decl_isr(28, irq_dispatcher);
__decl_isr(29, irq_dispatcher);
__decl_isr(30, irq_dispatcher);
__decl_isr(31, irq_dispatcher);
__decl_isr(32, irq_dispatcher);
__decl_isr(33, irq_dispatcher);
__decl_isr(34, irq_dispatcher);
__decl_isr(35, irq_dispatcher);
__decl_isr(36, irq_dispatcher);
__decl_isr(37, irq_dispatcher);
__decl_isr(38, irq_dispatcher);
__decl_isr(39, irq_dispatcher);
__decl_isr(40, irq_dispatcher);
__decl_isr(41, irq_dispatcher);
__decl_isr(42, irq_dispatcher);
__decl_isr(43, irq_dispatcher);
__decl_isr(44, irq_dispatcher);
__decl_isr(45, irq_dispatcher);
__decl_isr(46, irq_dispatcher);
__decl_isr(47, irq_dispatcher);


gate_t idt[irq::lines_number::value] _irq_aligned_;
table_pointer idt_pointer _irq_aligned_ = {sizeof(idt) - 1,
		static_cast<void *>(&idt)};

} // namespace

void irq::init() {
	// See comment__why_not_use_counter
	__decl_common_gate(0, idt);
	__decl_common_gate(1, idt);
	__decl_common_gate(2, idt);
	__decl_common_gate(3, idt);
	__decl_common_gate(4, idt);
	__decl_common_gate(5, idt);
	__decl_common_gate(6, idt);
	__decl_common_gate(7, idt);
	__decl_common_gate(8, idt);
	__decl_common_gate(9, idt);
	__decl_common_gate(10, idt);
	__decl_common_gate(11, idt);
	__decl_common_gate(12, idt);
	__decl_common_gate(13, idt);
	__decl_common_gate(14, idt);
	__decl_common_gate(15, idt);
	__decl_common_gate(16, idt);
	__decl_common_gate(17, idt);
	__decl_common_gate(18, idt);
	__decl_common_gate(19, idt);
	__decl_common_gate(20, idt);
	__decl_common_gate(21, idt);
	__decl_common_gate(22, idt);
	__decl_common_gate(23, idt);
	__decl_common_gate(24, idt);
	__decl_common_gate(25, idt);
	__decl_common_gate(26, idt);
	__decl_common_gate(27, idt);
	__decl_common_gate(28, idt);
	__decl_common_gate(29, idt);
	__decl_common_gate(30, idt);
	__decl_common_gate(31, idt);
	__decl_common_gate(32, idt);
	__decl_common_gate(33, idt);
	__decl_common_gate(34, idt);
	__decl_common_gate(35, idt);
	__decl_common_gate(36, idt);
	__decl_common_gate(37, idt);
	__decl_common_gate(38, idt);
	__decl_common_gate(39, idt);
	__decl_common_gate(40, idt);
	__decl_common_gate(41, idt);
	__decl_common_gate(42, idt);
	__decl_common_gate(43, idt);
	__decl_common_gate(44, idt);
	__decl_common_gate(45, idt);
	__decl_common_gate(46, idt);
	__decl_common_gate(47, idt);

	asm volatile("lidt %0"::"m" (idt_pointer));
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


void irq_dispatcher(irq::irq_t vector) {
	handlers_list_t &handlers = registered_isrs[vector];
	auto used_handler = lib::find_if(handlers.begin(), handlers.end(),
		[vector] (const irq::irq_handler_t &handler) -> bool {
			return handler(vector) ==
					irq::irq_return_t::handled;
	});
	if (used_handler == handlers.end()) {
		default_isr(vector);
	}
}


} // namespace
