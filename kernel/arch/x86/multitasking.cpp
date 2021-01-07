#include <arch/x86/multitasking.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/irq.hpp>

using namespace mmu;

void x86::switch_to(const uint16_t segment)
{
	uint16_t segment_ptr = segment * sizeof(GDTEntry);
	struct {unsigned int offset; unsigned short segment;} dest{};
	dest.offset = 0x0;
	dest.segment = segment_ptr; // whatever value you want in CS
	asm volatile (
	"ljmp *%0\n"
	"1:" :: "m"(dest));
}

lib::ostream& x86::operator<<(lib::ostream& out, const TaskStateSegment& tss)
{
	return out
		<< "tss{"
		<< "prev=" << tss.previous_task_link
		<< ",stack_0=" << tss.stack_0.stack_ptr << "/" << tss.stack_0.stack_segment
		<< ",isp=" << reinterpret_cast<uint32_t>(tss.instruction_ptr)
		<< ",eflags=" << tss.eflags
		<< ",cs=" << tss.cs.segment
		<< "}";
}

void x86::kernel_yield()
{
	switch_to(mmu::SegmentIndex::kernel_scheduler);
}


static struct
{
	lib::byte data[8 * 1024 * 4 * 10];
	lib::byte stack[0];
} scheduler_task_stack_storage;

[[noreturn]] void scheduler_routine();

x86::TaskStateSegment x86::scheduler_task{
	0,
	KERNEL_DATA_SEGMENT_POINTER,
	scheduler_task_stack_storage.stack,
	reinterpret_cast<lib::byte*>(&scheduler_routine),
	0x0,
	0x0, 0x0, 0x0, 0x0,
	scheduler_task_stack_storage.stack, scheduler_task_stack_storage.stack,
	0x0, 0x0,
	KERNEL_DATA_SEGMENT_POINTER, KERNEL_CODE_SEGMENT_POINTER
};

[[noreturn]] void scheduler_routine()
{
	irq::enable();
	lib::cnotice << "starting scheduling" << lib::endl;
	while (true) {
		x86::switch_to(SegmentIndex::kernel_other_task);
		x86::switch_to(SegmentIndex::kernel_other_task2);
	}
}

