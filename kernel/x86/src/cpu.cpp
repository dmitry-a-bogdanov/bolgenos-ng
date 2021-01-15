#include <x86/cpu.hpp>

#include <x86/segments.hpp>
#include <threading/threading.hpp>
#include <mutex.hpp>

void x86::Processor::load_kernel_segments()
{
	thr::with_lock<thr::RecursiveIrqLocker>([&]{
		_gdt.push_back(segments::null);
		_gdt.push_back(segments::kernel_code);
		_gdt.push_back(segments::kernel_data);
		_gdt.push_back(segments::single_task_descriptor);
		_gdt.reload_table();
		reload_segment_registers();
		init_task_register();
	});
}

void x86::Processor::load_interrupts_table()
{
	_idt.reload_table();
}

x86::EFlags x86::Processor::flags()
{
	EFlags result;
	asm volatile(
		"pushf \n"
		"pop %0"
		: "+m"(result)
		);
	return result;
}

void x86::Processor::switch_task_to(uint16_t segment_selector)
{
	struct {unsigned int offset; unsigned short segment;} dest{};
	dest.offset = 0x0;
	dest.segment = segment_selector;
	asm volatile (
	"ljmp *%0\n"
	"1:" :: "m"(dest));
}

