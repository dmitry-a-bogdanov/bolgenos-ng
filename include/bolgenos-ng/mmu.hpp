#pragma once


namespace mmu {


/// Index of null segment.
constexpr int NULL_SEGMENT_INDEX = 0;


/// Size of segment descriptor.
constexpr int SEGMENT_STRUCT_SIZE = 8;


/// Index of kernel code segment.
constexpr int KERNEL_CODE_SEGMENT_INDEX = 1;


/// Kernel code segment.
constexpr int KERNEL_CODE_SEGMENT_POINTER = KERNEL_CODE_SEGMENT_INDEX * SEGMENT_STRUCT_SIZE;


/// Index of kernel data segment.
constexpr int KERNEL_DATA_SEGMENT_INDEX = 2;


/// Kernel data segment.
constexpr int KERNEL_DATA_SEGMENT_POINTER = KERNEL_DATA_SEGMENT_INDEX * SEGMENT_STRUCT_SIZE;


/// \brief Setup segmentation support.
///
/// Setup segmentation support. The function initializes Global Descriptior
/// Table and loads it to appropriate CPU register.
void init();


} // namespace mmu
