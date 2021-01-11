#pragma once

#include "memory_segment_d.hpp"

namespace x86::segments {

extern const MemorySegmentDescriptor null;
extern const MemorySegmentDescriptor kernel_code;
extern const MemorySegmentDescriptor kernel_data;

}
