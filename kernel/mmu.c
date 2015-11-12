#include <bolgenos-ng/mmu.h>

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>

typedef enum {
	ssf_null = 0x0,
	ssf_system = 0x0,
	ssf_code_or_data = 0x1
} segment_system_flag_t;

typedef enum {
	dpl_null = 0x0,
	dpl_kernel = 0x0,
	dpl_user = 0x3
} dpl_t;

typedef enum {
	spf_null = 0x0,
	spf_not_present = 0x0,
	spf_present = 0x1
} segment_present_flag_t;

typedef enum {
	slf_null = 0x0,
	slf_64 = 0x1,
	slf_other = 0x0
} segment_long_flag_t;

typedef enum {
	sdbf_null = 0x0,
	sdbf_32_bit = 0x1,
	sdbf_16_bit = 0x0,
} segment_db_flag_t;

typedef enum {
	sgf_null = 0x0,
	sgf_4k_pages = 0x1,
	sgf_bytes = 0x0
} segment_granularity_flag_t;

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


typedef struct __attribute__((packed)) {
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
} segment_t;

#define __decl_segment(base_, limit_, type_, ssf_, dpl_, spf_, slf_,	\
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

check_type_size(segment_t, 8);

typedef struct __attribute__((packed)) {
	uint16_t limit:16;
	uint32_t base:32;
} gdt_pointer_t;

static segment_t __global_descriptor_table[] __attribute__((aligned(16))) = {
	// null-segment
	__decl_segment(0x0, 0x0, 0x0, ssf_null, dpl_null, spf_null, slf_null,
		sdbf_null, sgf_null),

	// kernel code segment
	__decl_segment(0x0, 0xfffff, st_code|st_code_read, ssf_code_or_data,
		dpl_kernel, spf_present, slf_other, sdbf_32_bit, sgf_4k_pages),

	// kernel data segment
	__decl_segment(0x0, 0xfffff, st_data|st_data_write, ssf_code_or_data,
		dpl_kernel, spf_present, slf_other, sdbf_32_bit, sgf_4k_pages)
};

void setup_segments() {
	static gdt_pointer_t gdt_pointer __attribute__((aligned(16)));
	gdt_pointer.limit = sizeof(__global_descriptor_table) - 1;
	gdt_pointer.base = (uint32_t) __global_descriptor_table;
	asm volatile("lgdt %0"::"m" (gdt_pointer));
}
