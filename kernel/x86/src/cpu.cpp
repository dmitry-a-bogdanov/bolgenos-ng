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
