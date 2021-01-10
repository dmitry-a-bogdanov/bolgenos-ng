#pragma once

#include "memory_segment_d.hpp"

namespace x86::segments {

extern const mmu::MemorySegmentDescriptor null;
extern const mmu::MemorySegmentDescriptor kernel_code;
extern const mmu::MemorySegmentDescriptor kernel_data;

}
