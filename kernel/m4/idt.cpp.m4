#include <m4/idt.hpp>
#include <bolgenos-ng/irq.hpp>

namespace {


m4_changequote([,])
m4_include(utils.m4)

m4_divert(-1)

m4_define([____asm], [____irqh_asm_$1])
m4_define([____c], [____irqh_c_$1])

m4_define([____asm_definition], [extern "C" void ____asm($1)();
  asm(
    ".align 16\n"
    "____asm($1):\n"
      "pushal\n"
      "push %esp\n"
      "pushl $$1\n"
      "call ____disp\n"
      "add $[]4, %esp\n"
      "pop %esp\n"
      "popal\n"
      "iret\n"
  );

])


m4_define([____irqh_definitions], [____for([____num], [0], [$1], [____asm_definition(____num)])])



m4_define([____fill_entry], [[$1][[$2]] = gate_t::interrupt_gate(____asm($2));
])

m4_define([____fill_idt], [____for([____num], 0, $2, [____fill_entry($1, ____num)])])

m4_divert[]m4_dnl

m4_define([__max_irq], 255)


/// Pointer to C++ IRQ dispatcher
m4::irq_dispatcher_func_t registered_dispatcher = nullptr;


/// C IRQ dispatcher. The function to be called from ASM handlers.
extern "C" void ____disp(irq::irq_t vector, void* frame) {
	registered_dispatcher(vector, frame);
}


____irqh_definitions(__max_irq)


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
			segment_(mmu::KERNEL_CODE_SEGMENT_POINTER),
			reserved_(0),
			zeros_(0),
			gate_kind_(gate_kind_type::interrupt),
			flag_32_bit_(bitness_flag_type::bits_32),
			zero_bit_(0),
			dpl_(x86::protection_ring_t::ring_null),
			present_(presence_type::presence_yes),
			offset_16_31_(0x0) {
	}


	explicit constexpr gate_t(func_addr_type func,
			gate_kind_type kind = gate_kind_type::trap)
			: offset_00_15_(bitmask(func, 0, 0xffff)),
			segment_(mmu::KERNEL_CODE_SEGMENT_POINTER),
			reserved_(0),
			zeros_(0),
			gate_kind_(kind),
			flag_32_bit_(bitness_flag_type::bits_32),
			zero_bit_(0),
			dpl_(x86::protection_ring_t::ring_kernel),
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
	x86::protection_ring_t	dpl_		:2;
	presence_type		present_	:1;
	func_addr_type		offset_16_31_	:16;
};

static_assert(sizeof(gate_t) == irq::GATE_SIZE, "gate_t has wrong size");


gate_t idt[[irq::NUMBER_OF_LINES]] _irq_aligned_;


} // namespace


void* m4::get_idt(irq_dispatcher_func_t function) {
	registered_dispatcher = function;
____fill_idt(idt, __max_irq)
	return &idt;
}

