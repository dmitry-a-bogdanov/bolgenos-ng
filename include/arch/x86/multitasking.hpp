#pragma once

#include <bolgenos-ng/asm.hpp>
#include <lib/ostream.hpp>
#include <cstddef.hpp>

namespace x86 {

struct __attribute__((packed)) TaskStateSegment {
	struct __attribute__((packed)) StackData {
		lib::byte*	stack_ptr{nullptr};
		uint16_t	stack_segment{0};
		uint16_t	reserved{0};
	};

	static_assert(sizeof(StackData) == 8);

	struct __attribute__((packed)) SegmentData {
		constexpr SegmentData(uint16_t segment_)
			: segment{segment_}
		{}
		uint16_t	segment;
		uint16_t	reserved{0};
	};

	static_assert(sizeof(SegmentData) == 4);

	constexpr TaskStateSegment(
		uint16_t prev_task_link_,
		uint16_t stack_segment_,
		lib::byte* stack_ptr_,
		lib::byte* instruction_ptr_,
		uint32_t eflags_,
		uint32_t eax_,
		uint32_t ecx_,
		uint32_t ebx_,
		uint32_t edx_,
		lib::byte* esp_,
		lib::byte* ebp_,
		uint32_t esi_,
		uint32_t edi_,
		uint16_t data_segment_ptr_,
		uint16_t code_segment_ptr_)
		:
		previous_task_link{prev_task_link_},
		stack_0{.stack_ptr = stack_ptr_, .stack_segment=stack_segment_},
		instruction_ptr{instruction_ptr_},
		eflags{eflags_},
		eax{eax_},
		ecx{ecx_},
		edx{edx_},
		ebx{ebx_},
		esp{esp_},
		ebp{ebp_},
		esi{esi_},
		edi{edi_},
		es{data_segment_ptr_},
		cs{code_segment_ptr_},
		ss{data_segment_ptr_},
		ds{data_segment_ptr_},
		fs{data_segment_ptr_},
		gs{data_segment_ptr_},
		ldt_segment_selector{0},
		debug_trap{0},
		reserved{0},
		io_map_base_address{0}
	{}

	uint16_t	previous_task_link;
	uint16_t	reserved_01{0};
	StackData	stack_0;
	StackData	stack_1{};
	StackData	stack_2{};
	/**
	 * \brief CR3
	 */
	lib::byte*	page_directory{nullptr};
	lib::byte*	instruction_ptr;
	uint32_t	eflags;
	uint32_t	eax;
	uint32_t	ecx;
	uint32_t	edx;
	uint32_t	ebx;
	lib::byte*	esp;
	lib::byte*	ebp;
	uint32_t	esi;
	uint32_t	edi;
	SegmentData	es;
	SegmentData	cs;
	SegmentData	ss;
	SegmentData	ds;
	SegmentData	fs;
	SegmentData	gs;
	SegmentData	ldt_segment_selector;
	uint16_t	debug_trap:1;
	uint16_t	reserved:15;
	uint16_t	io_map_base_address;
};

static_assert(sizeof(TaskStateSegment) == 104);

using TSS = TaskStateSegment;

struct TaskGate {
};

void switch_to(uint16_t segment);

lib::ostream& operator<<(lib::ostream& out, const TaskStateSegment& tss);

void kernel_yield();

extern TaskStateSegment scheduler_task;

} // namespace x86
