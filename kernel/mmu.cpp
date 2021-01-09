#include <bolgenos-ng/mmu.hpp>

#include <array.hpp>

#include <bolgenos-ng/compiler.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <x86/multitasking.hpp>
#include <m4/idt.hpp>
#include <bolgenos-ng/time.hpp>
#include <bolgenos-ng/memory.hpp>

using namespace mmu;
using namespace lib;

static_assert(sizeof(GDTEntry) == 8, "Wrong entry size");

[[noreturn]] void other_task_routine();
[[noreturn]] void other_task_routine2();

namespace Segments {

const GDTEntry null = { .memory_segment = {
	0x0,
	0x0,
	tag_type::st_null,
	seg_sys_flag_type::sys_null,
	protection_ring_t::ring_null,
	seg_present_type::present_null,
	seg_long_type::long_null,
	seg_db_type::db_null,
	seg_granularity_type::granularity_null
}};

const GDTEntry kernel_code = { .memory_segment = {
	0x0,
	0xfffff,
	static_cast<tag_type>(tag_type::code | tag_type::code_read),
	seg_sys_flag_type::code_or_data,
	protection_ring_t::ring_kernel,
	seg_present_type::present,
	seg_long_type::other,
	seg_db_type::db32_bit,
	seg_granularity_type::four_k_pages
}};

const GDTEntry kernel_data = { .memory_segment = {
	0x0,
	0xfffff,
	static_cast<tag_type>(tag_type::data | tag_type::data_write),
	seg_sys_flag_type::code_or_data,
	protection_ring_t::ring_kernel,
	seg_present_type::present,
	seg_long_type::other,
	seg_db_type::db32_bit,
	seg_granularity_type::four_k_pages
}};

}


static_assert(lib::is_standard_layout_v<GDTEntry>,
	"GDTEntry is not std layout");

static_assert(lib::is_trivial_v<GDTEntry>,
	"GDTEntry is not trivial");

static_assert(lib::is_pod_v<GDTEntry>,
	"GDTEntry is not POD");


/// \brief Global Descriptor Table.
///
/// Array represents global descriptor table for the system. Note,
/// that it has zero overhead for filling it as structs, this initializers
/// works during compile-time.
alignas(cpu_alignment)
static lib::Array<GDTEntry, SegmentIndex::number_of_segments> gdt{
	Segments::null,
	Segments::kernel_code,
	Segments::kernel_data
};

static_assert(memory::is_aligned_at_least<cpu_alignment>(gdt.data()));

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

void mmu::init() {
	{
		auto& task = x86::tasks[SegmentIndex::kernel_scheduler - SegmentIndex::first_task_index];
		task.tss = x86::TSS{
			0,
			KERNEL_DATA_SEGMENT_POINTER,
			task.stack,
			reinterpret_cast<byte*>(scheduler_routine),
			0x0,
			x86::tss::GPRegistersPack::zero_on_stack(task.stack),
			x86::tss::SegmentRegistersPack::kernel()
		};
		task.available = false;
	}

	{
		auto& task = x86::tasks[SegmentIndex::kernel_other_task - SegmentIndex::first_task_index];
		task.tss = x86::TSS{
			0,
			KERNEL_DATA_SEGMENT_POINTER,
			task.stack,
			reinterpret_cast<lib::byte*>(&other_task_routine),
			0x0,
			x86::tss::GPRegistersPack::zero_on_stack(task.stack),
			x86::tss::SegmentRegistersPack::kernel()
		};
		task.available = false;
	}

	{
		auto& task = x86::tasks[SegmentIndex::kernel_other_task2 - SegmentIndex::first_task_index];
		task.tss = x86::TSS{
			0,
			KERNEL_DATA_SEGMENT_POINTER,
			task.stack,
			reinterpret_cast<lib::byte*>(&other_task_routine2),
			0x0,
			x86::tss::GPRegistersPack::zero_on_stack(task.stack),
			x86::tss::SegmentRegistersPack::kernel()
		};
		task.available = false;
	}

	for (size_t segment_idx = SegmentIndex::first_task_index;
		segment_idx <= (SegmentIndex::first_task_index + x86::TASKS);
		++segment_idx)
	{
		size_t task_idx = segment_idx - SegmentIndex::first_task_index;
		gdt[segment_idx].task =  {
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
	gdtp.base = gdt.data();
	asm volatile("lgdt %0"::"m" (gdtp));
	reload_segments();
}


[[noreturn]] void other_task_routine() {
	irq::enable();
	lib::cwarn << "inside the first task!" << lib::endl;

	uint16_t counter = 0;
	while (true) {
		lib::cnotice << "task[1]: " << ++counter << lib::endl;
		sleep_ms(100);
		x86::kernel_yield();
	}
}


[[noreturn]] void other_task_routine2() {
	irq::enable();
	lib::cwarn << "inside the second task!" << lib::endl;

	uint16_t counter = 0;
	while (true) {
		lib::cnotice << "task[2]: " << ++counter << lib::endl;
		sleep_ms(100);
		x86::kernel_yield();
	}
}

