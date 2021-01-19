project(libx86)

add_library(libx86 STATIC
	include/x86/cpu.hpp
	include/x86/gate.hpp
	include/x86/gdt.hpp
	include/x86/idt.hpp
	include/x86/gate.hpp
	include/x86/memory_segment_d.hpp
	include/x86/segment_flags.hpp
	include/x86/segments.hpp
	include/x86/tssd.hpp
	include/x86/tss.hpp

	src/cpu.cpp
	src/eflags.cpp
	src/gate.cpp
	src/gdt.cpp
	src/idt.cpp
	src/irq.cpp
	src/memory_segment_d.cpp
	src/segments.cpp
	src/traps.cpp
	src/traps.hpp
	src/tss.cpp
	src/tssd.cpp
)

target_include_directories(libx86 PUBLIC include/)
target_link_libraries(libx86 PUBLIC
	libkernelcxx libthreading sched interrupt_controller)
