#ifndef __BOLGENOS_NG__MMU_H__
#define __BOLGENOS_NG__MMU_H__

/**
* \brief Kernel code segment
*
* Segment of kernel code. Segment with index 1. The next after null-segment.
*/
#define KERNEL_CS		(0x8)


/**
* \brief Kernel data segment
*
* Segment of kernel data. Segment with index 2. The next after kernel code
*	segment.
*/
#define KERNEL_DS		(0x10)


/**
* \brief Setup segmentation support.
*
* Setup segmentation support. The function initializes Global Descriptior
*	Table and loads it to appropriate CPU register.
*/
void setup_segments();

#endif // __BOLGENOS_NG__MMU_H__
