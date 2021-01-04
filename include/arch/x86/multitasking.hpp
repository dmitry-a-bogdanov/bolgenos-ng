#pragma once

#include <bolgenos-ng/asm.hpp>
#include <cstddef.hpp>

namespace x86 {

struct __attribute__((packed)) TaskStateSegment {
	struct __attribute__((packed)) StackData {
		lib::byte*	stack_ptr;
		uint16_t	stack_segment;
		uint16_t	reserved;
	};

	static_assert(sizeof(StackData) == 8);

	struct __attribute__((packed)) SegmentData {
		uint16_t	segment;
		uint16_t	reserved;
	};

	static_assert(sizeof(SegmentData) == 4);

	uint16_t	previous_task_link;
	uint16_t	reserved_01;
	StackData	stacks_0;
	StackData	stacks_1;
	StackData	stacks_2;
	/**
	 * \brief CR3
	 */
	lib::byte*	page_directory;
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

void switch_to(const TaskGate&);

} // namespace x86
