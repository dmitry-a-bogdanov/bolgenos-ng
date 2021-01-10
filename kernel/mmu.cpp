#include <bolgenos-ng/mmu.hpp>

using namespace mmu;
using namespace lib;


static_assert(8 == mmu::KERNEL_CODE_SEGMENT_SELECTOR, "Wrong kernel code segment");
static_assert(16 == mmu::KERNEL_DATA_SEGMENT_SELECTOR, "Wrong kernel data segment");

