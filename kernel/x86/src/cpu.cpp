#include <x86/cpu.hpp>

#include <x86/segments.hpp>

void x86::Processor::load_kernel_segments()
{
	_gdt.push_back(segments::null);
	_gdt.push_back(segments::kernel_code);
	_gdt.push_back(segments::kernel_data);
	_gdt.push_back(segments::single_task_descriptor);
	_gdt.reload_table();
	reload_segment_registers();
	init_task_register();
}
