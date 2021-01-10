#include <x86/tss.hpp>

#include <bolgenos-ng/mmu.hpp>
#include <ostream.hpp>
#include <ext/scoped_format_guard.hpp>

using namespace lib;

// Compile-time checks
static_assert(sizeof(x86::tss::ProtectionRingStack) == 8);
static_assert(sizeof(x86::tss::SegmentRegister) == 4);
static_assert(sizeof(x86::TaskStateSegment) == 104);


lib::ostream& x86::tss::operator<<(ostream& out, const x86::tss::SegmentRegister& segment_register)
{
	return out << segment_register.segment();
}

ostream& x86::tss::operator<<(ostream& out, const x86::tss::ProtectionRingStack& stack)
{
	ScopedFormatGuard guard{out};
	return out << stack.segment() << ":" << hex << static_cast<const void*>(stack.pointer());
}

lib::ostream& x86::operator<<(lib::ostream& out, const TaskStateSegment& tss)
{
	lib::ScopedFormatGuard guard{out};
	return out << hex
		<< "tss{"
		<< "prev=" << tss.previous_task_link
		<< ",stack_0=" << tss.stack[0]
		<< ",isp=" << reinterpret_cast<uint32_t>(tss.instruction_ptr)
		<< ",eflags=" << tss.eflags
		<< ",cs=" << tss._segment_registers.cs
		<< "}";
}

x86::tss::SegmentRegistersPack x86::tss::SegmentRegistersPack::kernel()
{
	return x86::tss::SegmentRegistersPack(
		mmu::KERNEL_CODE_SEGMENT_SELECTOR,
		mmu::KERNEL_DATA_SEGMENT_SELECTOR
	);
}
