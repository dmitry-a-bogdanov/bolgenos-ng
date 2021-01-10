#include <x86/segments.hpp>

using namespace mmu;

const MemorySegmentDescriptor x86::segments::null{
	0x0,
	0x0,
	tag_type::st_null,
	seg_sys_flag_type::sys_null,
	protection_ring_t::ring_null,
	seg_present_type::present_null,
	seg_long_type::long_null,
	seg_db_type::db_null,
	seg_granularity_type::granularity_null
};

const MemorySegmentDescriptor x86::segments::kernel_code{
	0x0,
	0xfffff,
	static_cast<tag_type>(tag_type::code | tag_type::code_read),
	seg_sys_flag_type::code_or_data,
	protection_ring_t::ring_kernel,
	seg_present_type::present,
	seg_long_type::other,
	seg_db_type::db32_bit,
	seg_granularity_type::four_k_pages
};

const MemorySegmentDescriptor x86::segments::kernel_data{
	0x0,
	0xfffff,
	static_cast<tag_type>(tag_type::data | tag_type::data_write),
	seg_sys_flag_type::code_or_data,
	protection_ring_t::ring_kernel,
	seg_present_type::present,
	seg_long_type::other,
	seg_db_type::db32_bit,
	seg_granularity_type::four_k_pages
};

