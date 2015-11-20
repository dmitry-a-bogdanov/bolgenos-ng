#include <bolgenos-ng/irq.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/mmu.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.h>
#include <bolgenos-ng/vga_console.h>

#define __align_for_irq__	aligned(16)

#define __macro_concat(x, y) x ## y
#define macro_concat(x, y) __macro_concat(x, y)


typedef uint64_t gate_field_t;

#define __reserved(size) gate_field_t macro_concat(__reserved__, __LINE__) : size

// Common gate type that can alias every other gate in strict terms.
// As the result we must use the same type for all gate fields.
typedef struct __attribute__((packed)) {
	__reserved(16);
	__reserved(16);
	__reserved(5);
	__reserved(3);
	__reserved(3);
	__reserved(1);
	__reserved(1);
	__reserved(2);
	__reserved(1);
	__reserved(16);
} gate_t;

check_type_size(gate_t, 8);

#define GATE_TASK	(0x5)
#define GATE_INT	(0x6)
#define GATE_TRAP	(0x7)

#define __as_gate(in) ((gate_t *)(in))

typedef struct __attribute__((packed)) {
	__reserved(16);
	gate_field_t tss_segment: 16;
	__reserved(5);
	__reserved(3);
	gate_field_t gate_type : 3;
	gate_field_t task_zero : 1;
	__reserved(1);
	gate_field_t dpl : 2;
	gate_field_t present : 1;
	__reserved(16);
} task_gate_t;

check_type_size(task_gate_t, 8);

// Comment__why_using_get_gate:
//
// Strict aliasing rules are only applied to pointer and cannot be applied
// to lvalue of some non-pointer type. This constraint include also explicit
// typecasting after getting address. As the result, workaround with
// casting function is need. However, it doesn't mean that code violates
// strict alising rules, because types are still compatible.
static inline gate_t *task_get_gate(task_gate_t *task) {
	return __as_gate(task);
}

typedef struct __attribute__((packed)) {
	gate_field_t offset_00_15: 16;
	gate_field_t segment: 16;
	__reserved(5);
	gate_field_t zeros :3;
	gate_field_t gate_type : 3;
	gate_field_t flag_32_bit : 1;
	gate_field_t zero_bit : 1;
	gate_field_t dpl : 2;
	gate_field_t present : 1;
	gate_field_t offset_16_31 : 16;
} int_gate_t;

check_type_size(int_gate_t, 8);

// see Comment__why_using_get_gate
static inline gate_t *int_get_gate(int_gate_t *intr) {
	return __as_gate(intr);
}

#define __decl_int_gate(offset_, segment_) {				\
	.offset_00_15 = bitmask(offset_, 0, 0xffff),			\
	.segment = segment_,						\
	.zeros = 0x0,							\
	.gate_type = GATE_INT,						\
	.flag_32_bit = 1,						\
	.zero_bit = 0,							\
	.dpl = 0x0,							\
	.present = 1,							\
	.offset_16_31 = bitmask(offset_, 16, 0xffff)			\
}

typedef struct __attribute__((packed)) {
	gate_field_t offset_00_15: 16;
	gate_field_t segment: 16;
	__reserved(5);
	gate_field_t zeros :3;
	gate_field_t gate_type : 3;
	gate_field_t flag_32_bit : 1;
	gate_field_t zero_bit : 1;
	gate_field_t dpl : 2;
	gate_field_t present : 1;
	gate_field_t offset_16_31 : 16;
} trap_gate_t;

#define __decl_trap_gate(offset_, segment_) {				\
	.offset_00_15 = bitmask(offset_, 0, 0xffff),			\
	.segment = segment_,						\
	.zeros = 0x0,							\
	.gate_type = GATE_TRAP,						\
	.flag_32_bit = 1,						\
	.zero_bit = 0,							\
	.dpl = 0x0,							\
	.present = 1,							\
	.offset_16_31 = bitmask(offset_, 16, 0xffff)			\
}

// See Comment__why_using_get_gate
static inline gate_t *trap_get_gate(trap_gate_t *trap) {
	return __as_gate(trap);
}

#define __decl_empty_gate() {						\
	.present = 0							\
}

check_type_size(trap_gate_t, 8);


#define __decl_isr_stage_asm(num)					\
	asm(								\
		".align 16\n"						\
		"__isr_stage_asm_" #num ":\n"				\
			"pushal\n"					\
			"call __isr_stage_c_" #num "\n"			\
			"popal\n"					\
			"iret\n"					\
	);								\
	__link_from_asm__ void __isr_stage_asm_ ## num()


#define __decl_isr_stage_c(num, generic_isr)				\
	void __attribute__((used)) __isr_stage_c_ ## num () {		\
		generic_isr(num);					\
		system_pic->end_of_interrupt(num);			\
	}

#define __decl_isr(num, function)					\
	__decl_isr_stage_asm(num);					\
	__decl_isr_stage_c(num, function);


#define __decl_common_gate(num, table)					\
	do {								\
		int_gate_t gate = __decl_int_gate(			\
			(uint32_t) __isr_stage_asm_ ## num, KERNEL_CS);	\
		table[num] = *int_get_gate(&gate);			\
	} while(0)


static void __generic_isr(uint8_t irq) {
	switch(irq) {
	case 0x20:
		handle_timer_interrupt();
		break;
	default:
		vga_console_puts("generic irq\n");
		break;
	}
}


// comment__why_not_use_counter:
//
// Looks like using __COUNTER__ is unsafe since it may depends on
// compilation order.
__decl_isr(0, __generic_isr);
__decl_isr(1, __generic_isr);
__decl_isr(2, __generic_isr);
__decl_isr(3, __generic_isr);
__decl_isr(4, __generic_isr);
__decl_isr(5, __generic_isr);
__decl_isr(6, __generic_isr);
__decl_isr(7, __generic_isr);
__decl_isr(8, __generic_isr);
__decl_isr(9, __generic_isr);
__decl_isr(10, __generic_isr);
__decl_isr(11, __generic_isr);
__decl_isr(12, __generic_isr);
__decl_isr(13, __generic_isr);
__decl_isr(14, __generic_isr);
__decl_isr(15, __generic_isr);
__decl_isr(16, __generic_isr);
__decl_isr(17, __generic_isr);
__decl_isr(18, __generic_isr);
__decl_isr(19, __generic_isr);
__decl_isr(20, __generic_isr);
__decl_isr(21, __generic_isr);
__decl_isr(22, __generic_isr);
__decl_isr(23, __generic_isr);
__decl_isr(24, __generic_isr);
__decl_isr(25, __generic_isr);
__decl_isr(26, __generic_isr);
__decl_isr(27, __generic_isr);
__decl_isr(28, __generic_isr);
__decl_isr(29, __generic_isr);
__decl_isr(30, __generic_isr);
__decl_isr(31, __generic_isr);
__decl_isr(32, __generic_isr);
__decl_isr(33, __generic_isr);
__decl_isr(34, __generic_isr);
__decl_isr(35, __generic_isr);
__decl_isr(36, __generic_isr);
__decl_isr(37, __generic_isr);
__decl_isr(38, __generic_isr);
__decl_isr(39, __generic_isr);
__decl_isr(40, __generic_isr);
__decl_isr(41, __generic_isr);
__decl_isr(42, __generic_isr);
__decl_isr(43, __generic_isr);
__decl_isr(44, __generic_isr);
__decl_isr(45, __generic_isr);
__decl_isr(46, __generic_isr);
__decl_isr(47, __generic_isr);


static gate_t idt[NUMBER_OF_IRQS] __attribute__((__align_for_irq__));
static descriptor_table_ptr_t idt_pointer __attribute__((__align_for_irq__));

void setup_interrupts() {
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

	idt_pointer.limit = sizeof(idt) - 1;
	idt_pointer.base = (uint32_t) idt;
	asm volatile("lidt %0"::"m" (idt_pointer));
}
