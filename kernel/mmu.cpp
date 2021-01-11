#include <bolgenos-ng/mmu.hpp>

static_assert(8 == x86::KERNEL_CODE_SEGMENT_SELECTOR, "Wrong kernel code segment");
static_assert(16 == x86::KERNEL_DATA_SEGMENT_SELECTOR, "Wrong kernel data segment");

