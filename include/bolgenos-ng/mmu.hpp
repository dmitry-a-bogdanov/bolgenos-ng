#pragma once

#include <lib/type_traits.hpp>


namespace mmu {


/// Size of segment descriptor.
using struct_seg_size = lib::integral_constant<int, 8>;


/// Index of kernel code segment.
using kernel_cs_idx = lib::integral_constant<int, 1>;


/// Kernel code segment.
using kernel_cs_ptr = lib::integral_constant<int,
		kernel_cs_idx::value * struct_seg_size::value>;


/// \brief Setup segmentation support.
///
/// Setup segmentation support. The function initializes Global Descriptior
/// Table and loads it to appropriate CPU register.
void init();


} // namespace mmu
