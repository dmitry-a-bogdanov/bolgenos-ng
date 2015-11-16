#include <bolgenos-ng/irq.h>

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/vga_console.h>

#define IRQ_VECTORS_NUMBER	(256)

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

#define ret_from_interrupt() asm ("iret\n")

void __attribute__((used)) __dummy_int_function() {
	vga_console_puts("Got interrupt\n");
};

extern void __dummy_interrupt_handler();

asm(	"__dummy_interrupt_handler:\n"
	"pushal\n"
	"call __dummy_int_function\n"
	"popal\n"
	"iret\n");

//void __dummy_interrupt_handler() {
//	asm ("pushal");
//	asm ("cld");
//	vga_console_puts("Got interrupt\n");
//	do {
//		asm ("hlt");
//	} while(1);
//	ret_from_interrupt();
//	asm ("popal");
//	asm ("iret");
//}

/*
void __dummy_fill_idt(gate_t *idt_table) {
	memset(idt_table, 0, IRQ_VECTORS_NUMBER * sizeof(gate_t));
	int_gate_t int_gate_tmp = __decl_int_gate((uint32_t)((void*)__dummy_interrput_handler), 0x8);
	union {
		int_gate_t int_gate;
		gate_t gate;
	} __std_int_gate;
	__std_int_gate.int_gate = int_gate_tmp;
	for (int i = 0; i < IRQ_VECTORS_NUMBER; ++i) {
		idt_table[i] = __std_int_gate.gate;
	}
}
*/

static const uint32_t handler_addr = (uint32_t) ((void *) __dummy_interrupt_handler);

#include <bolgenos-ng/vga_console.h>
#include <bolgenos-ng/string.h>

void setup_interrupts() {
	static gate_t idt_table[256];
		//__attribute__((aligned(16)));
	int_gate_t intr = __decl_int_gate(handler_addr, 0x08);
	trap_gate_t trap = __decl_trap_gate(handler_addr, 0x08);
	(void) trap;
	for (int i = 0; i < 256; ++i) {
		idt_table[i] = *as_gate(&intr);
	}

	static idt_pointer_t idt_pointer;// __attribute__((aligned(16)));
	idt_pointer.limit = sizeof(idt_table) - 1;
	idt_pointer.base = (uint32_t) idt_table;
	asm volatile("lidt %0"::"m" (idt_pointer));
	char debug_info[100];
	char *print_ptr = debug_info;

	print_ptr += uint32_to_string(*((uint32_t*)(void *)&idt_table[8]), print_ptr, 16);
	//print_ptr += uint32_to_string(1, print_ptr, 16);
	*print_ptr++ = ' ';
	print_ptr += uint32_to_string(*((uint32_t*) (4 + (void *)&idt_table[8])), print_ptr, 16);
	*print_ptr++ = '\n';
	*print_ptr++ = '\0';
	vga_console_puts(debug_info);
}
