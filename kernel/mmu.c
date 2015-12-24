#include <bolgenos-ng/mmu.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/printk.h>


/**
* \brief Memory segment: system flag
*
* Enum holds values of system flag for memory segment descriptor.
*/
typedef enum {
	ssf_null		= 0x0, /*!< NULL-segments */
	ssf_system		= 0x0, /*!< System segment */
	ssf_code_or_data	= 0x1, /*!< Code or data segments */
} segment_system_flag_t;


/**
* \brief Memory segment: privilege level.
*
* Enum holds values of privilege level for memory segment descriptor.
*/
typedef enum {
	dpl_null		= 0x0, /*!< NULL-segments */
	dpl_kernel		= 0x0, /*!< Kernel segments */
	dpl_user		= 0x3, /*!< Userspace segments */
} dpl_t;


/**
* \brief Memory segment: present flag.
*
* Enum holds values of present flag for memory segment descriptor.
*/
typedef enum {
	spf_null		= 0x0, /*!< NULL-segment */
	spf_not_present		= 0x0, /*!< Segment is not present */
	spf_present		= 0x1, /*!< Segment is present */
} segment_present_flag_t;


/**
* \brief Memory segment: long flag.
*
* Enum holds values of long flag for memory segment descriptor.
*/
typedef enum {
	slf_null		= 0x0, /*!< NULL-segment */
	slf_64			= 0x1, /*!< 64-bit segment */
	slf_other		= 0x0, /*!< 32-bit segment */
} segment_long_flag_t;


typedef enum {
	sdbf_null = 0x0,
	sdbf_32_bit = 0x1,
	sdbf_16_bit = 0x0,
} segment_db_flag_t;


/**
* \brief Memory segment: granularity flag.
*
* Enum holds values of granularity flag for memory segment descriptor.
*/
typedef enum {
	sgf_null		= 0x0, /*!< NULL-segment */
	sgf_4k_pages		= 0x1, /*!< Use 4K pages */
	sgf_bytes		= 0x0, /*!< Do not use pages */
} segment_granularity_flag_t;


/**
* \brief Memory segment: D/B flag.
*
* \warning hard to document this option without good description of
*	segmentation mechanism.
*
* TODO: document this shit.
*/
typedef enum {
	st_null				= 0 << 0,
// common
	st_accessed			= 1 << 0,

// data & data-specific flags
	st_data				= 0 << 0,
	st_data_write			= 1 << 1,
	st_data_down			= 1 << 2,

// code & code-specific flags
	st_code				= 1 << 3,
	st_code_read			= 1 << 1,
	st_code_conforming		= 1 << 2,
} segment_type_t;


struct _packed_ seg {
	uint16_t				limit_00_15		:16;
	uint16_t				base_00_15		:16;
	uint8_t					base_16_23		 :8;
	segment_type_t				type			 :4;
	segment_system_flag_t			system_flag		 :1;
	dpl_t					dpl			 :2;
	segment_present_flag_t			present_flag		 :1;
	uint8_t					limit_16_19		 :4;
	uint8_t					__avl			 :1;
	segment_long_flag_t			long_flag		 :1;
	segment_db_flag_t			db_flag			 :1;
	segment_granularity_flag_t		granularity_flag	 :1;
	uint8_t					base_24_31		 :8;
};


#define SEG_INITIALIZER(base_, limit_, type_, ssf_, dpl_, spf_, slf_,	\
		       sdbf_, sgf_ ) {					\
	.limit_00_15 = bitmask(limit_, 0, 0xffff),			\
	.base_00_15 = bitmask(base_, 0, 0xffff),			\
	.base_16_23 = bitmask(base_, 16, 0xff),				\
	.type = type_,							\
	.system_flag = ssf_,						\
	.dpl = dpl_,							\
	.present_flag = spf_,						\
	.limit_16_19 = bitmask(limit_, 16, 0xf),			\
	.__avl = 0,							\
	.long_flag = slf_,						\
	.db_flag = sdbf_,						\
	.granularity_flag = sgf_,					\
	.base_24_31 = bitmask(base_, 24, 0xff),				\
}

assert_type_size(struct seg, STRUCT_SEG_SIZE);

static struct seg gdt[] _mmu_aligned_ = {
	[__NULL_SEGMENT] = SEG_INITIALIZER(0x0, 0x0, 0x0, ssf_null, dpl_null,
		spf_null, slf_null, sdbf_null, sgf_null),

	[__KERNEL_CS] = SEG_INITIALIZER(0x0, 0xfffff, st_code|st_code_read,
		ssf_code_or_data, dpl_kernel, spf_present, slf_other,
		sdbf_32_bit, sgf_4k_pages),

	[__KERNEL_DS] = SEG_INITIALIZER(0x0, 0xfffff, st_data|st_data_write,
		ssf_code_or_data, dpl_kernel, spf_present, slf_other,
		sdbf_32_bit, sgf_4k_pages),
};


static struct table_pointer gdtp _mmu_aligned_;

void setup_segments() {
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = gdt;
	asm volatile("lgdt %0"::"m" (gdtp));
}
