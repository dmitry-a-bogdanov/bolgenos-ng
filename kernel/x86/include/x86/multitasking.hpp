#pragma once

#include <cstddef.hpp>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/page.hpp>

#include "tss.hpp"

namespace lib {
class ostream;
}

namespace x86 {


struct Task {
	TSS tss{};
	bool available = true;
	lib::byte _stack_storage[PAGE_SIZE*16]{};
	lib::byte stack[0];
	uint16_t segment_selector{0};
	char name[16]{"<unknown>"};


	void clear_stack() {
		for (auto& byte: _stack_storage) {
			byte = static_cast<lib::byte>(0xaa);
		}
	}
};

lib::ostream& operator<<(lib::ostream& out, const Task& task);

} // namespace x86
