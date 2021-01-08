#pragma once

#include <cstddef.hpp>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/page.hpp>
#include <lib/ostream.hpp>

#include "tss.hpp"

namespace x86 {

void switch_to(uint16_t segment);

void kernel_yield();


struct Task {
	TSS tss{};
	bool available = true;
	lib::byte stack[PAGE_SIZE*16];
};

constexpr size_t TASKS = 128;
extern Task tasks[TASKS];

inline Task& get_task(const size_t i) {
	return tasks[i];
}
inline Task* get_task_ptr(const size_t i) {
	return tasks + i;
}

} // namespace x86

[[noreturn]] void scheduler_routine();
