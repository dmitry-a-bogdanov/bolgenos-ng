#include <bolgenos-ng/mmu.hpp>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <arch/x86/multitasking.hpp>
#include <bolgenos-ng/kernel_object.hpp>
#include <lib/ostream.hpp>
#include <m4/idt.hpp>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.hpp>

using namespace mmu;
using namespace lib;

static_assert(SEGMENT_STRUCT_SIZE == 8, "Segment has wrong size");
static_assert(sizeof(GDTEntry) == 8, "Wrong entry size");
static_assert(sizeof(TaskStateSegmentDescriptor) == 8, "Wrong size");

static x86::TaskStateSegment main_task{
	0,
	KERNEL_DATA_SEGMENT_POINTER,
	static_cast<lib::byte*>(kobj::stack_begin()),
	nullptr,
	0x0,
	0x0, 0x0, 0x0, 0x0,
	nullptr, nullptr,
	0x0, 0x0,
	KERNEL_DATA_SEGMENT_POINTER, KERNEL_CODE_SEGMENT_POINTER
	};


static struct
{
	lib::byte data[8 * 1024 * 4 * 10];
	lib::byte stack[0];
} other_stack_storage, other_stack_storage2;

extern "C" [[noreturn]] void other_task_routine();
extern "C" [[noreturn]] void other_task_routine2();



/// \brief Global Descriptor Table.
///
/// Array represents global descriptor table for the system. Note,
/// that it has zero overhead for filling it as structs, this initializers
/// works during compile-time.
static GDTEntry gdt[SegmentIndex::last_task_index + 1] _mmu_aligned_ = {
	[mmu::SegmentIndex::null] = {.memory_segment = {
			0x0,
			0x0,
			tag_type::st_null,
			seg_sys_flag_type::sys_null,
			protection_ring_t::ring_null,
			seg_present_type::present_null,
			seg_long_type::long_null,
			seg_db_type::db_null,
			seg_granularity_type::granularity_null
	}},
	[mmu::SegmentIndex::kernel_code] = {.memory_segment = {
			0x0,
			0xfffff,
			static_cast<tag_type>(tag_type::code | tag_type::code_read),
			seg_sys_flag_type::code_or_data,
			protection_ring_t::ring_kernel,
			seg_present_type::present,
			seg_long_type::other,
			seg_db_type::db32_bit,
			seg_granularity_type::four_k_pages
	}},
	[mmu::SegmentIndex::kernel_data] = {.memory_segment = {
			0x0,
			0xfffff,
			static_cast<tag_type>(tag_type::data | tag_type::data_write),
			seg_sys_flag_type::code_or_data,
			protection_ring_t::ring_kernel,
			seg_present_type::present,
			seg_long_type::other,
			seg_db_type::db32_bit,
			seg_granularity_type::four_k_pages
	}}
	/*
	[mmu::SegmentIndex::kernel_scheduler
		... (mmu::SegmentIndex::kernel_scheduler + x86::TASKS)] = {.task_descriptor = {
		reinterpret_cast<uint32_t>(static_cast<void *>(&x86::scheduler_task)),
		sizeof(x86::TaskStateSegment) - 1,
		false,
		protection_ring_t::ring_kernel,
		seg_present_type::present,
		seg_granularity_type::bytes
	}},
	[mmu::SegmentIndex::kernel_other_task] = {.task_descriptor = {
		reinterpret_cast<uint32_t>(static_cast<void *>(&other_task)),
		sizeof(x86::TaskStateSegment) - 1,
		false,
		protection_ring_t::ring_kernel,
		seg_present_type::present,
		seg_granularity_type::bytes
	}},
	[mmu::SegmentIndex::kernel_other_task2] = {.task_descriptor = {
		reinterpret_cast<uint32_t>(static_cast<void *>(&other_task2)),
		sizeof(x86::TaskStateSegment) - 1,
		false,
		protection_ring_t::ring_kernel,
		seg_present_type::present,
		seg_granularity_type::bytes
	}},*/
};


/**
* \brief Global Descriptor Table Pointer.
*
* Pointer to Global Descriptor Table that is used by x86 CPU to load GTD.
*/
static struct table_pointer gdtp _mmu_aligned_;

/**
*
* Changing in segment descriptor takes effect only when segment register
* is set.
*/
static void reload_segments() {
    static_assert(8 == mmu::KERNEL_CODE_SEGMENT_POINTER, "Wrong kernel code segment");
    static_assert(16 == mmu::KERNEL_DATA_SEGMENT_POINTER, "Wrong kernel data segment");

	asm volatile(
		"movw %w0, %%ds\n"
		"movw %w0, %%es\n"
		"movw %w0, %%ss\n"
		"movw %w0, %%fs\n"
		"movw %w0, %%gs\n"
		"ljmp %w1, $__update_cs\n"
		"__update_cs:\n"
		: // no outputs
		: "r" (mmu::KERNEL_DATA_SEGMENT_POINTER), "i" (mmu::KERNEL_CODE_SEGMENT_POINTER)
		: "cc", "memory"
		);
}

static struct
{
	lib::byte data[8 * 1024 * 4 * 10];
	lib::byte stack[0];
} scheduler_task_stack_storage;


void mmu::init() {
	x86::tasks[SegmentIndex::kernel_scheduler - SegmentIndex::first_task_index].tss = x86::TSS{
		0,
		KERNEL_DATA_SEGMENT_POINTER,
		scheduler_task_stack_storage.stack,
		reinterpret_cast<byte*>(scheduler_routine),
		0x0,
		0x0, 0x0, 0x0, 0x0,
		scheduler_task_stack_storage.stack, scheduler_task_stack_storage.stack,
		0x0, 0x0,
		KERNEL_DATA_SEGMENT_POINTER, KERNEL_CODE_SEGMENT_POINTER
	};

	x86::tasks[SegmentIndex::kernel_other_task - SegmentIndex::first_task_index].tss = x86::TSS{
		0,
		KERNEL_DATA_SEGMENT_POINTER,
		other_stack_storage.stack,
		reinterpret_cast<lib::byte*>(&other_task_routine),
		0x0,
		0x0, 0x0, 0x0, 0x0,
		other_stack_storage.stack, other_stack_storage.stack,
		0x0, 0x0,
		KERNEL_DATA_SEGMENT_POINTER, KERNEL_CODE_SEGMENT_POINTER
	};

	x86::tasks[SegmentIndex::kernel_other_task2 - SegmentIndex::first_task_index].tss = x86::TSS{
		0,
		KERNEL_DATA_SEGMENT_POINTER,
		other_stack_storage2.stack,
		reinterpret_cast<lib::byte*>(&other_task_routine2),
		0x0,
		0x0, 0x0, 0x0, 0x0,
		other_stack_storage2.stack, other_stack_storage2.stack,
		0x0, 0x0,
		KERNEL_DATA_SEGMENT_POINTER, KERNEL_CODE_SEGMENT_POINTER
	};

	for (size_t segment_idx = SegmentIndex::first_task_index;
		segment_idx <= (SegmentIndex::first_task_index + x86::TASKS);
		++segment_idx)
	{
		size_t task_idx = segment_idx - SegmentIndex::first_task_index;
		gdt[segment_idx].task_descriptor =  {
			reinterpret_cast<uint32_t>(
				static_cast<void *>(x86::get_task_ptr(task_idx))),
			sizeof(x86::Task) - 1,
			false,
			protection_ring_t::ring_kernel,
			seg_present_type::present,
			seg_granularity_type::bytes
		};
	}

	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = gdt;
	asm volatile("lgdt %0"::"m" (gdtp));
	reload_segments();
}


extern "C" [[noreturn]] void other_task_routine() {
	irq::enable();
	lib::cwarn << "inside the first task!" << lib::endl;

	uint32_t current_task_gate;

	asm volatile("str %0\n\t"
	: "=m"(current_task_gate)
	:
	: "memory");

	const uint32_t gate_idx = current_task_gate/sizeof(GDTEntry);

	lib::cwarn
		<< "task gate ptr: " << current_task_gate << ", "
		<< "index in gdt:" << gate_idx << lib::endl;

	lib::cnotice << "doing the second switch" << endl;

	asm volatile(
	"ljmp $40, $switched_task_ok\n\t"
	"switched_task_ok:\n\t");

	uint16_t counter = 0;
	while (true) {
		lib::cnotice << "task[1]: " << ++counter << lib::endl;
		sleep_ms(500);
		x86::kernel_yield();
	}
}


extern "C" [[noreturn]] void other_task_routine2() {
	irq::enable();
	lib::cwarn << "inside the second task!" << lib::endl;

	uint32_t current_task_gate;

	asm volatile("str %0\n\t"
	: "=m"(current_task_gate)
	:
	: "memory");

	const uint32_t gate_idx = current_task_gate/sizeof(GDTEntry);

	lib::cwarn
		<< "task gate ptr: " << current_task_gate << ", "
		<< "index in gdt:" << gate_idx << lib::endl;

	uint16_t counter = 0;
	while (true) {
		lib::cnotice << "task[2]: " << ++counter << lib::endl;
		sleep_ms(500);
		x86::kernel_yield();
	}
}

