#include <bolgenos-ng/mmu.hpp>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/stdtypes.hpp>



/// \brief Memory segment: system flag
///
/// Enum holds values of system flag for memory segment descriptor.
enum seg_sys_flag_type {
	/// NULL-segment
	sys_null	= 0x0,


	/// System segment
	system		= 0x0,


	 /// Code or data segments
	code_or_data	= 0x1,
};





/**
* \brief Memory segment: present flag.
*
* Enum holds values of present flag for memory segment descriptor.
*/
enum seg_present_type {
	present_null		= 0x0, /*!< NULL-segment */
	not_present		= 0x0, /*!< Segment is not present */
	present			= 0x1, /*!< Segment is present */
};


/**
* \brief Memory segment: long flag.
*
* Enum holds values of long flag for memory segment descriptor.
*/
enum seg_long_type {
	long_null		= 0x0, /*!< NULL-segment */
	long_64			= 0x1, /*!< 64-bit segment */
	other		= 0x0, /*!< 32-bit segment */
};


enum seg_db_type {
	db_null = 0x0,
	db32_bit = 0x1,
	db16_bit = 0x0,
};


/**
* \brief Memory segment: granularity flag.
*
* Enum holds values of granularity flag for memory segment descriptor.
*/
enum seg_granularity_type {
	granularity_null		= 0x0, /*!< NULL-segment */
	four_k_pages			= 0x1, /*!< Use 4K pages */
	bytes				= 0x0, /*!< Do not use pages */
};


enum seg_avl_type {
	avl_null = 0x0
};


/// \brief Segment descriptor.
///
/// Structure that provides fine interface for filling segment descriptor.
struct _packed_ Segment {
public:
	/// \brief Memory segment: D/B flag.
	///
	/// \warning hard to document this option without good description of
	/// segmentation mechanism.
	///
	/// TODO: document this shit.
	enum tag_type {
		st_null				= 0 << 0,
	// common
		accessed			= 1 << 0,

	// data & data-specific flags
		data				= 0 << 3,
		data_write			= 1 << 1,
		data_down			= 1 << 2,

	// code & code-specific flags
		code				= 1 << 3,
		code_read			= 1 << 1,
		code_conforming		= 1 << 2,
	};

	constexpr Segment(uint32_t base,
			uint32_t limit,
			tag_type tag,
			seg_sys_flag_type ssf,
			protection_ring_t dpl,
			seg_present_type present,
			seg_long_type lng,
			seg_db_type db,
			seg_granularity_type granularity)
			:
			limit_00_15_(bitmask(limit, 0, 0xffff)),
			base_00_15_(bitmask(base, 0, 0xffff)),
			base_16_23_(bitmask(base, 16, 0xff)),
			tag_(tag),
			system_flag_(ssf),
			dpl_(dpl),
			present_(present),
			limit_16_19_(bitmask(limit, 16, 0xf)),
			avl_(seg_avl_type::avl_null),
			long_(lng),
			db_(db),
			granularity_(granularity),
			base_24_31_(bitmask(base, 24, 0xff))
		{
	}

private:
	uint16_t			limit_00_15_		:16;
	uint16_t			base_00_15_		:16;
	uint8_t				base_16_23_		 :8;
	tag_type			tag_			 :4;
	seg_sys_flag_type		system_flag_		 :1;
	protection_ring_t		dpl_			 :2;
	seg_present_type		present_		 :1;
	uint8_t				limit_16_19_		 :4;
	seg_avl_type			avl_			 :1;
	seg_long_type			long_			 :1;
	seg_db_type			db_			 :1;
	seg_granularity_type		granularity_		 :1;
	uint8_t				base_24_31_		 :8;
};

static_assert(sizeof(Segment) == mmu::SEGMENT_STRUCT_SIZE, "Segment has wrong size");


/// \brief Global Descriptor Table.
///
/// Array represents global descriptor table for the system. Note,
/// that it has zero overhead for filling it as structs, this initializers
/// works during compile-time.
Segment gdt[] _mmu_aligned_ = {
	[mmu::NULL_SEGMENT_INDEX] = {
			0x0,
			0x0,
			Segment::tag_type::st_null,
			seg_sys_flag_type::sys_null,
			protection_ring_t::ring_null,
			seg_present_type::present_null,
			seg_long_type::long_null,
			seg_db_type::db_null,
			seg_granularity_type::granularity_null
	},
	[mmu::KERNEL_CODE_SEGMENT_INDEX] = {
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
	},
	[mmu::KERNEL_DATA_SEGMENT_INDEX] = {
			0x0,
			0xfffff,
			static_cast<Segment::tag_type>(Segment::tag_type::data|Segment::tag_type::data_write),
			seg_sys_flag_type::code_or_data,
			protection_ring_t::ring_kernel,
			seg_present_type::present,
			seg_long_type::other,
			seg_db_type::db32_bit,
			seg_granularity_type::four_k_pages
	}
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

#define __kern_ds 16
static_assert(__kern_ds == mmu::KERNEL_DATA_SEGMENT_POINTER, "Wrong kernel data segment");


#define __kern_cs 8
static_assert(__kern_cs == mmu::KERNEL_CODE_SEGMENT_POINTER, "Wrong kernel code segment");


	asm volatile(
		"movw $" stringify(__kern_ds) ", %ax\n"
		"movw %ax, %ds\n"
		"movw %ax, %es\n"
		"movw %ax, %ss\n"
		"movw %ax, %fs\n"
		"movw %ax, %gs\n"
		"ljmp $" stringify(__kern_cs) ", $__update_cs\n"
		"__update_cs:\n"
		);


#undef __kern_cs
#undef __kern_ds
}


void mmu::init() {
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = gdt;
	asm volatile("lgdt %0"::"m" (gdtp));
	reload_segments();
}
