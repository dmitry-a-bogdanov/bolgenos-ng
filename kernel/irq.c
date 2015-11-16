#include <bolgenos-ng/irq.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/mmu.h>

#define IRQ_VECTORS_NUMBER	(256)

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

#define as_gate(in) ((gate_t *)(in))

typedef struct __attribute__((packed)) {
	__reserved(16);
	gate_field_t tss_segment: 16;
	__reserved(5);
	__reserved(3);
	gate_field_t gate_type : 3;
	gate_field_t task_zero : 1;

	gate_field_t dpl : 2;
	gate_field_t present : 1;
	__reserved(16);
} task_gate_t;


check_type_size(task_gate_t, 8);

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

#define __decl_int_gate(offset_, segment_) {				\
	.offset_00_15 = bitmask(offset_, 0, 0xff),			\
	.segment = segment_,						\
	.zeros = 0x0,							\
	.gate_type = GATE_INT,						\
	.flag_32_bit = 1,						\
	.zero_bit = 0,							\
	.dpl = 0x0,							\
	.present = 1,							\
	.offset_16_31 = bitmask(offset_, 16, 0xff)			\
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
	.offset_00_15 = bitmask(offset_, 0, 0xff),			\
	.segment = segment_,						\
	.zeros = 0x0,							\
	.gate_type = GATE_TRAP,						\
	.flag_32_bit = 1,						\
	.zero_bit = 0,							\
	.dpl = 0x0,							\
	.present = 1,							\
	.offset_16_31 = bitmask(offset_, 16, 0xff)			\
}

#define __decl_empty_gate() {						\
	.present = 0							\
}

check_type_size(trap_gate_t, 8);

typedef struct __attribute__((packed)) {
	uint16_t limit:16;
	uint32_t base:32;
} idt_pointer_t;

#include <bolgenos-ng/vga_console.h>
void __attribute__((used)) __dummy_int_function() {
	vga_console_puts("Got interrupt\n");
};


__link_from_asm__ void __dummy_interrupt_handler();
asm(
"__dummy_interrupt_handler:\n"
	"pushal\n"
	"call __dummy_int_function\n"
	"popal\n"
	"iret\n");


static gate_t idt[IRQ_VECTORS_NUMBER] __attribute__((__align_for_irq__));
static descriptor_table_ptr_t idt_pointer __attribute__((__align_for_irq__));

void setup_interrupts() {
	uint32_t empty_handler_addr =
			(uint32_t) ((void *) __dummy_interrupt_handler);
	int_gate_t intr = __decl_int_gate(empty_handler_addr, KERNEL_CS);
	for (int i = 0; i < IRQ_VECTORS_NUMBER; ++i) {
		idt[i] = *as_gate(&intr);
	}

	idt_pointer.limit = sizeof(idt) - 1;
	idt_pointer.base = (uint32_t) idt;
	asm volatile("lidt %0"::"m" (idt_pointer));
}
