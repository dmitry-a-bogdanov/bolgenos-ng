#include <x86/multitasking.hpp>
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


void x86::kernel_yield()
{
	switch_to(mmu::SegmentIndex::kernel_scheduler);
}


[[noreturn]] void scheduler_routine()
{
	irq::enable();
	lib::cnotice << "starting scheduling" << lib::endl;
	while (true) {
		x86::switch_to(SegmentIndex::kernel_other_task);
		x86::switch_to(SegmentIndex::kernel_other_task2);
	}
}

alignas(PAGE_SIZE)
x86::Task x86::tasks[x86::TASKS];

lib::ostream& x86::operator<<(lib::ostream& out, const Task& task) {
	return out << "{.tss = " << task.tss << "}";
}

