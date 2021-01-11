#pragma once

#include "memory_segment_d.hpp"
#include "tss.hpp"
#include "tssd.hpp"

namespace x86::segments {

extern const MemorySegmentDescriptor null;
extern const MemorySegmentDescriptor kernel_code;
extern const MemorySegmentDescriptor kernel_data;

extern const TaskStateSegment single_task;
extern const TaskStateSegmentDescriptor single_task_descriptor;

}
