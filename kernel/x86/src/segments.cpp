#include <x86/segments.hpp>

using namespace x86;

const MemorySegmentDescriptor x86::segments::null{
	0x0,
	0x0,
	SegmentType::null,
	System::null,
	ProtectionRing::null,
	Present::null,
	Long::null,
	OperationSize::null,
	Granularity::null
};

const MemorySegmentDescriptor x86::segments::kernel_code{
	0x0,
	0xfffff,
	static_cast<SegmentType>(SegmentType::code | SegmentType::code_read),
	System::code_or_data,
	ProtectionRing::kernel,
	Present::present,
	Long::other,
	OperationSize::db32_bit,
	Granularity::four_k_pages
};

const MemorySegmentDescriptor x86::segments::kernel_data{
	0x0,
	0xfffff,
	static_cast<SegmentType>(SegmentType::data | SegmentType::data_write),
	System::code_or_data,
	ProtectionRing::kernel,
	Present::present,
	Long::other,
	OperationSize::db32_bit,
	Granularity::four_k_pages
};

