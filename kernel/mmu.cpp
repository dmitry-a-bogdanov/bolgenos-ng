#include <bolgenos-ng/mmu.hpp>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/mem_utils.hpp>

using namespace mmu;

static_assert(SEGMENT_STRUCT_SIZE == 8, "Segment has wrong size");
static_assert(sizeof(GDTEntry) == 8, "Wrong entry size");


/// \brief Global Descriptor Table.
///
/// Array represents global descriptor table for the system. Note,
/// that it has zero overhead for filling it as structs, this initializers
/// works during compile-time.
GDTEntry gdt[] _mmu_aligned_ = {
	[mmu::SegmentIndex::null] = {.memory_segment = {
			0x0,
			0x0,
			Segment::tag_type::st_null,
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
			static_cast<Segment::tag_type>(
					Segment::tag_type::code |
					Segment::tag_type::code_read),
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
			static_cast<Segment::tag_type>(Segment::tag_type::data|Segment::tag_type::data_write),
			seg_sys_flag_type::code_or_data,
			protection_ring_t::ring_kernel,
			seg_present_type::present,
			seg_long_type::other,
			seg_db_type::db32_bit,
			seg_granularity_type::four_k_pages
	}},
	[mmu::SegmentIndex::kernel_main_task] = {.memory_segment = {
		0x0,
		0xfffff,
		static_cast<Segment::tag_type>(0x7),
		seg_sys_flag_type::system,
		protection_ring_t::ring_kernel,
		seg_present_type::present,
		seg_long_type::other,
		seg_db_type::db32_bit,
		seg_granularity_type::four_k_pages
	}}
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


void mmu::init() {
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = gdt;
	asm volatile("lgdt %0"::"m" (gdtp));
	reload_segments();
}
