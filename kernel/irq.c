#include <bolgenos-ng/irq.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/mmu.h>
#include <bolgenos-ng/pic_8259.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.h>
#include <bolgenos-ng/vga_console.h>

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

#define __as_gate(in) ((gate_t *)(in))

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
		pic_8259_send_end_of_interrput(num);			\
	}

#define __decl_isr(num, function)					\
	__decl_isr_stage_asm(num);					\
	__decl_isr_stage_c(num, function);


#define __decl_common_gate(num, table)					\
	do {								\
		int_gate_t gate = __decl_int_gate(			\
			(uint32_t) __isr_stage_asm_ ## num, KERNEL_CS);	\
		table[num] = *__as_gate(&gate);				\
	} while(0)

#define EMPTY_HANDLER 0
static void handle_timer_interrupt() {
#if EMPTY_HANDLER == 1
	++ticks;
	vga_console_puts("timer irq\n");
#else
	char info[20];
	char *pptr = info;
	pptr += uint32_to_string(ticks, pptr, 10);
	*pptr = '\n';
	++pptr;
	*pptr = '\0';
	vga_console_puts(info);
	++ticks;
#endif
}

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
__decl_isr(48, __generic_isr);
__decl_isr(49, __generic_isr);
__decl_isr(50, __generic_isr);
__decl_isr(51, __generic_isr);
__decl_isr(52, __generic_isr);
__decl_isr(53, __generic_isr);
__decl_isr(54, __generic_isr);
__decl_isr(55, __generic_isr);
__decl_isr(56, __generic_isr);
__decl_isr(57, __generic_isr);
__decl_isr(58, __generic_isr);
__decl_isr(59, __generic_isr);
__decl_isr(60, __generic_isr);
__decl_isr(61, __generic_isr);
__decl_isr(62, __generic_isr);
__decl_isr(63, __generic_isr);
__decl_isr(64, __generic_isr);
__decl_isr(65, __generic_isr);
__decl_isr(66, __generic_isr);
__decl_isr(67, __generic_isr);
__decl_isr(68, __generic_isr);
__decl_isr(69, __generic_isr);
__decl_isr(70, __generic_isr);
__decl_isr(71, __generic_isr);
__decl_isr(72, __generic_isr);
__decl_isr(73, __generic_isr);
__decl_isr(74, __generic_isr);
__decl_isr(75, __generic_isr);
__decl_isr(76, __generic_isr);
__decl_isr(77, __generic_isr);
__decl_isr(78, __generic_isr);
__decl_isr(79, __generic_isr);
__decl_isr(80, __generic_isr);
__decl_isr(81, __generic_isr);
__decl_isr(82, __generic_isr);
__decl_isr(83, __generic_isr);
__decl_isr(84, __generic_isr);
__decl_isr(85, __generic_isr);
__decl_isr(86, __generic_isr);
__decl_isr(87, __generic_isr);
__decl_isr(88, __generic_isr);
__decl_isr(89, __generic_isr);
__decl_isr(90, __generic_isr);
__decl_isr(91, __generic_isr);
__decl_isr(92, __generic_isr);
__decl_isr(93, __generic_isr);
__decl_isr(94, __generic_isr);
__decl_isr(95, __generic_isr);
__decl_isr(96, __generic_isr);
__decl_isr(97, __generic_isr);
__decl_isr(98, __generic_isr);
__decl_isr(99, __generic_isr);
__decl_isr(100, __generic_isr);
__decl_isr(101, __generic_isr);
__decl_isr(102, __generic_isr);
__decl_isr(103, __generic_isr);
__decl_isr(104, __generic_isr);
__decl_isr(105, __generic_isr);
__decl_isr(106, __generic_isr);
__decl_isr(107, __generic_isr);
__decl_isr(108, __generic_isr);
__decl_isr(109, __generic_isr);
__decl_isr(110, __generic_isr);
__decl_isr(111, __generic_isr);
__decl_isr(112, __generic_isr);
__decl_isr(113, __generic_isr);
__decl_isr(114, __generic_isr);
__decl_isr(115, __generic_isr);
__decl_isr(116, __generic_isr);
__decl_isr(117, __generic_isr);
__decl_isr(118, __generic_isr);
__decl_isr(119, __generic_isr);
__decl_isr(120, __generic_isr);
__decl_isr(121, __generic_isr);
__decl_isr(122, __generic_isr);
__decl_isr(123, __generic_isr);
__decl_isr(124, __generic_isr);
__decl_isr(125, __generic_isr);
__decl_isr(126, __generic_isr);
__decl_isr(127, __generic_isr);
__decl_isr(128, __generic_isr);
__decl_isr(129, __generic_isr);
__decl_isr(130, __generic_isr);
__decl_isr(131, __generic_isr);
__decl_isr(132, __generic_isr);
__decl_isr(133, __generic_isr);
__decl_isr(134, __generic_isr);
__decl_isr(135, __generic_isr);
__decl_isr(136, __generic_isr);
__decl_isr(137, __generic_isr);
__decl_isr(138, __generic_isr);
__decl_isr(139, __generic_isr);
__decl_isr(140, __generic_isr);
__decl_isr(141, __generic_isr);
__decl_isr(142, __generic_isr);
__decl_isr(143, __generic_isr);
__decl_isr(144, __generic_isr);
__decl_isr(145, __generic_isr);
__decl_isr(146, __generic_isr);
__decl_isr(147, __generic_isr);
__decl_isr(148, __generic_isr);
__decl_isr(149, __generic_isr);
__decl_isr(150, __generic_isr);
__decl_isr(151, __generic_isr);
__decl_isr(152, __generic_isr);
__decl_isr(153, __generic_isr);
__decl_isr(154, __generic_isr);
__decl_isr(155, __generic_isr);
__decl_isr(156, __generic_isr);
__decl_isr(157, __generic_isr);
__decl_isr(158, __generic_isr);
__decl_isr(159, __generic_isr);
__decl_isr(160, __generic_isr);
__decl_isr(161, __generic_isr);
__decl_isr(162, __generic_isr);
__decl_isr(163, __generic_isr);
__decl_isr(164, __generic_isr);
__decl_isr(165, __generic_isr);
__decl_isr(166, __generic_isr);
__decl_isr(167, __generic_isr);
__decl_isr(168, __generic_isr);
__decl_isr(169, __generic_isr);
__decl_isr(170, __generic_isr);
__decl_isr(171, __generic_isr);
__decl_isr(172, __generic_isr);
__decl_isr(173, __generic_isr);
__decl_isr(174, __generic_isr);
__decl_isr(175, __generic_isr);
__decl_isr(176, __generic_isr);
__decl_isr(177, __generic_isr);
__decl_isr(178, __generic_isr);
__decl_isr(179, __generic_isr);
__decl_isr(180, __generic_isr);
__decl_isr(181, __generic_isr);
__decl_isr(182, __generic_isr);
__decl_isr(183, __generic_isr);
__decl_isr(184, __generic_isr);
__decl_isr(185, __generic_isr);
__decl_isr(186, __generic_isr);
__decl_isr(187, __generic_isr);
__decl_isr(188, __generic_isr);
__decl_isr(189, __generic_isr);
__decl_isr(190, __generic_isr);
__decl_isr(191, __generic_isr);
__decl_isr(192, __generic_isr);
__decl_isr(193, __generic_isr);
__decl_isr(194, __generic_isr);
__decl_isr(195, __generic_isr);
__decl_isr(196, __generic_isr);
__decl_isr(197, __generic_isr);
__decl_isr(198, __generic_isr);
__decl_isr(199, __generic_isr);
__decl_isr(200, __generic_isr);
__decl_isr(201, __generic_isr);
__decl_isr(202, __generic_isr);
__decl_isr(203, __generic_isr);
__decl_isr(204, __generic_isr);
__decl_isr(205, __generic_isr);
__decl_isr(206, __generic_isr);
__decl_isr(207, __generic_isr);
__decl_isr(208, __generic_isr);
__decl_isr(209, __generic_isr);
__decl_isr(210, __generic_isr);
__decl_isr(211, __generic_isr);
__decl_isr(212, __generic_isr);
__decl_isr(213, __generic_isr);
__decl_isr(214, __generic_isr);
__decl_isr(215, __generic_isr);
__decl_isr(216, __generic_isr);
__decl_isr(217, __generic_isr);
__decl_isr(218, __generic_isr);
__decl_isr(219, __generic_isr);
__decl_isr(220, __generic_isr);
__decl_isr(221, __generic_isr);
__decl_isr(222, __generic_isr);
__decl_isr(223, __generic_isr);
__decl_isr(224, __generic_isr);
__decl_isr(225, __generic_isr);
__decl_isr(226, __generic_isr);
__decl_isr(227, __generic_isr);
__decl_isr(228, __generic_isr);
__decl_isr(229, __generic_isr);
__decl_isr(230, __generic_isr);
__decl_isr(231, __generic_isr);
__decl_isr(232, __generic_isr);
__decl_isr(233, __generic_isr);
__decl_isr(234, __generic_isr);
__decl_isr(235, __generic_isr);
__decl_isr(236, __generic_isr);
__decl_isr(237, __generic_isr);
__decl_isr(238, __generic_isr);
__decl_isr(239, __generic_isr);
__decl_isr(240, __generic_isr);
__decl_isr(241, __generic_isr);
__decl_isr(242, __generic_isr);
__decl_isr(243, __generic_isr);
__decl_isr(244, __generic_isr);
__decl_isr(245, __generic_isr);
__decl_isr(246, __generic_isr);
__decl_isr(247, __generic_isr);
__decl_isr(248, __generic_isr);
__decl_isr(249, __generic_isr);
__decl_isr(250, __generic_isr);
__decl_isr(251, __generic_isr);
__decl_isr(252, __generic_isr);
__decl_isr(253, __generic_isr);
__decl_isr(254, __generic_isr);
__decl_isr(255, __generic_isr);


static gate_t idt[IRQ_VECTORS_NUMBER] __attribute__((__align_for_irq__));
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
	__decl_common_gate(48, idt);
	__decl_common_gate(49, idt);
	__decl_common_gate(50, idt);
	__decl_common_gate(51, idt);
	__decl_common_gate(52, idt);
	__decl_common_gate(53, idt);
	__decl_common_gate(54, idt);
	__decl_common_gate(55, idt);
	__decl_common_gate(56, idt);
	__decl_common_gate(57, idt);
	__decl_common_gate(58, idt);
	__decl_common_gate(59, idt);
	__decl_common_gate(60, idt);
	__decl_common_gate(61, idt);
	__decl_common_gate(62, idt);
	__decl_common_gate(63, idt);
	__decl_common_gate(64, idt);
	__decl_common_gate(65, idt);
	__decl_common_gate(66, idt);
	__decl_common_gate(67, idt);
	__decl_common_gate(68, idt);
	__decl_common_gate(69, idt);
	__decl_common_gate(70, idt);
	__decl_common_gate(71, idt);
	__decl_common_gate(72, idt);
	__decl_common_gate(73, idt);
	__decl_common_gate(74, idt);
	__decl_common_gate(75, idt);
	__decl_common_gate(76, idt);
	__decl_common_gate(77, idt);
	__decl_common_gate(78, idt);
	__decl_common_gate(79, idt);
	__decl_common_gate(80, idt);
	__decl_common_gate(81, idt);
	__decl_common_gate(82, idt);
	__decl_common_gate(83, idt);
	__decl_common_gate(84, idt);
	__decl_common_gate(85, idt);
	__decl_common_gate(86, idt);
	__decl_common_gate(87, idt);
	__decl_common_gate(88, idt);
	__decl_common_gate(89, idt);
	__decl_common_gate(90, idt);
	__decl_common_gate(91, idt);
	__decl_common_gate(92, idt);
	__decl_common_gate(93, idt);
	__decl_common_gate(94, idt);
	__decl_common_gate(95, idt);
	__decl_common_gate(96, idt);
	__decl_common_gate(97, idt);
	__decl_common_gate(98, idt);
	__decl_common_gate(99, idt);
	__decl_common_gate(100, idt);
	__decl_common_gate(101, idt);
	__decl_common_gate(102, idt);
	__decl_common_gate(103, idt);
	__decl_common_gate(104, idt);
	__decl_common_gate(105, idt);
	__decl_common_gate(106, idt);
	__decl_common_gate(107, idt);
	__decl_common_gate(108, idt);
	__decl_common_gate(109, idt);
	__decl_common_gate(110, idt);
	__decl_common_gate(111, idt);
	__decl_common_gate(112, idt);
	__decl_common_gate(113, idt);
	__decl_common_gate(114, idt);
	__decl_common_gate(115, idt);
	__decl_common_gate(116, idt);
	__decl_common_gate(117, idt);
	__decl_common_gate(118, idt);
	__decl_common_gate(119, idt);
	__decl_common_gate(120, idt);
	__decl_common_gate(121, idt);
	__decl_common_gate(122, idt);
	__decl_common_gate(123, idt);
	__decl_common_gate(124, idt);
	__decl_common_gate(125, idt);
	__decl_common_gate(126, idt);
	__decl_common_gate(127, idt);
	__decl_common_gate(128, idt);
	__decl_common_gate(129, idt);
	__decl_common_gate(130, idt);
	__decl_common_gate(131, idt);
	__decl_common_gate(132, idt);
	__decl_common_gate(133, idt);
	__decl_common_gate(134, idt);
	__decl_common_gate(135, idt);
	__decl_common_gate(136, idt);
	__decl_common_gate(137, idt);
	__decl_common_gate(138, idt);
	__decl_common_gate(139, idt);
	__decl_common_gate(140, idt);
	__decl_common_gate(141, idt);
	__decl_common_gate(142, idt);
	__decl_common_gate(143, idt);
	__decl_common_gate(144, idt);
	__decl_common_gate(145, idt);
	__decl_common_gate(146, idt);
	__decl_common_gate(147, idt);
	__decl_common_gate(148, idt);
	__decl_common_gate(149, idt);
	__decl_common_gate(150, idt);
	__decl_common_gate(151, idt);
	__decl_common_gate(152, idt);
	__decl_common_gate(153, idt);
	__decl_common_gate(154, idt);
	__decl_common_gate(155, idt);
	__decl_common_gate(156, idt);
	__decl_common_gate(157, idt);
	__decl_common_gate(158, idt);
	__decl_common_gate(159, idt);
	__decl_common_gate(160, idt);
	__decl_common_gate(161, idt);
	__decl_common_gate(162, idt);
	__decl_common_gate(163, idt);
	__decl_common_gate(164, idt);
	__decl_common_gate(165, idt);
	__decl_common_gate(166, idt);
	__decl_common_gate(167, idt);
	__decl_common_gate(168, idt);
	__decl_common_gate(169, idt);
	__decl_common_gate(170, idt);
	__decl_common_gate(171, idt);
	__decl_common_gate(172, idt);
	__decl_common_gate(173, idt);
	__decl_common_gate(174, idt);
	__decl_common_gate(175, idt);
	__decl_common_gate(176, idt);
	__decl_common_gate(177, idt);
	__decl_common_gate(178, idt);
	__decl_common_gate(179, idt);
	__decl_common_gate(180, idt);
	__decl_common_gate(181, idt);
	__decl_common_gate(182, idt);
	__decl_common_gate(183, idt);
	__decl_common_gate(184, idt);
	__decl_common_gate(185, idt);
	__decl_common_gate(186, idt);
	__decl_common_gate(187, idt);
	__decl_common_gate(188, idt);
	__decl_common_gate(189, idt);
	__decl_common_gate(190, idt);
	__decl_common_gate(191, idt);
	__decl_common_gate(192, idt);
	__decl_common_gate(193, idt);
	__decl_common_gate(194, idt);
	__decl_common_gate(195, idt);
	__decl_common_gate(196, idt);
	__decl_common_gate(197, idt);
	__decl_common_gate(198, idt);
	__decl_common_gate(199, idt);
	__decl_common_gate(200, idt);
	__decl_common_gate(201, idt);
	__decl_common_gate(202, idt);
	__decl_common_gate(203, idt);
	__decl_common_gate(204, idt);
	__decl_common_gate(205, idt);
	__decl_common_gate(206, idt);
	__decl_common_gate(207, idt);
	__decl_common_gate(208, idt);
	__decl_common_gate(209, idt);
	__decl_common_gate(210, idt);
	__decl_common_gate(211, idt);
	__decl_common_gate(212, idt);
	__decl_common_gate(213, idt);
	__decl_common_gate(214, idt);
	__decl_common_gate(215, idt);
	__decl_common_gate(216, idt);
	__decl_common_gate(217, idt);
	__decl_common_gate(218, idt);
	__decl_common_gate(219, idt);
	__decl_common_gate(220, idt);
	__decl_common_gate(221, idt);
	__decl_common_gate(222, idt);
	__decl_common_gate(223, idt);
	__decl_common_gate(224, idt);
	__decl_common_gate(225, idt);
	__decl_common_gate(226, idt);
	__decl_common_gate(227, idt);
	__decl_common_gate(228, idt);
	__decl_common_gate(229, idt);
	__decl_common_gate(230, idt);
	__decl_common_gate(231, idt);
	__decl_common_gate(232, idt);
	__decl_common_gate(233, idt);
	__decl_common_gate(234, idt);
	__decl_common_gate(235, idt);
	__decl_common_gate(236, idt);
	__decl_common_gate(237, idt);
	__decl_common_gate(238, idt);
	__decl_common_gate(239, idt);
	__decl_common_gate(240, idt);
	__decl_common_gate(241, idt);
	__decl_common_gate(242, idt);
	__decl_common_gate(243, idt);
	__decl_common_gate(244, idt);
	__decl_common_gate(245, idt);
	__decl_common_gate(246, idt);
	__decl_common_gate(247, idt);
	__decl_common_gate(248, idt);
	__decl_common_gate(249, idt);
	__decl_common_gate(250, idt);
	__decl_common_gate(251, idt);
	__decl_common_gate(252, idt);
	__decl_common_gate(253, idt);
	__decl_common_gate(254, idt);
	__decl_common_gate(255, idt);

	idt_pointer.limit = sizeof(idt) - 1;
	idt_pointer.base = (uint32_t) idt;
	asm volatile("lidt %0"::"m" (idt_pointer));
}
