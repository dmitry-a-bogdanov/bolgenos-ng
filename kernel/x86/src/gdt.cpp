#include <x86/gdt.hpp>

#include <bolgenos-ng/asm.hpp>
#include <log.hpp>
#include <bolgenos-ng/memory.hpp>
#include <x86/segment_flags.hpp>

using namespace lib;
using namespace x86;

static_assert(8 == x86::KERNEL_CODE_SELECTOR, "Wrong kernel code segment");
static_assert(16 == x86::KERNEL_DATA_SELECTOR, "Wrong kernel data segment");

x86::GDT::GDT()
{
	static_assert(sizeof(Entry) == 8);
	static_assert(lib::is_trivial_v<Entry>);
	static_assert(lib::is_standard_layout_v<Entry>);
}


void x86::GDT::reload_table()
{
	cinfo << "reloading gdt" << endl;
	_gdt_pointer.limit = _gdt.size() * sizeof(Entry) - 1;
	_gdt_pointer.base = _gdt.data();
	asm volatile("lgdt %0"::"m" (_gdt_pointer));
	cinfo << "gdt was reloaded" << endl;
}

uint16_t x86::GDT::push_back(const MemorySegmentDescriptor& msd)
{
	uint16_t idx = _gdt.size();
	_gdt.push_back({.memory_segment = msd});
	return idx;
}

uint16_t x86::GDT::push_back(const TaskStateSegmentDescriptor& tssd)
{
	uint16_t idx = _gdt.size();
	_gdt.push_back({.task = tssd});
	return idx;
}

[[gnu::noinline]] void x86::reload_segment_registers()
{
	const uint16_t data_segment = segment_selector(SegmentIndex::kernel_data,
						       TableIndicator::GLOBAL,
						       ProtectionRing::kernel);
	const uint16_t code_segment = segment_selector(SegmentIndex::kernel_code,
						       TableIndicator::GLOBAL,
						       ProtectionRing::kernel);
	asm volatile(
	"movw %w0, %%ds\n"
	"movw %w0, %%es\n"
	"movw %w0, %%ss\n"
	"movw %w0, %%fs\n"
	"movw %w0, %%gs\n"
	"ljmp %w1, $update_cs____\n"
	"update_cs____:\n"
	: // no outputs
	: "r" (data_segment), "i" (code_segment)
	: "cc", "memory"
	);
}

void x86::init_task_register()
{
	asm volatile(
		"ltr %0\n\t"
		:
		: "r"(segment_selector(SegmentIndex::single_task, TableIndicator::GLOBAL,
		     ProtectionRing::kernel))
		: "cc", "memory"
		);
}

