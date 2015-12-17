#ifndef __BOLGENOS_NG__MMU_H__
#define __BOLGENOS_NG__MMU_H__

/**
* Size of segment descriptor.
*/
#define __SEGMENT_DESCR_SIZE		(8)


/**
* Index of null segment.
*/
#define __NULL_SEGMENT			(0x0)


/**
* Null segment.
*/
#define NULL_SEGMENT			(__NULL_SEGMENT * __SEGMENT_DESCR_SIZE)


/**
* Index of kernel code segment.
*/
#define __KERNEL_CS			(1)


/**
* Kernel code segment.
*/
#define KERNEL_CS			(__KERNEL_CS * __SEGMENT_DESCR_SIZE)


/**
* Index of kernel data segment.
*/
#define __KERNEL_DS			(2)

/**
* Kernel data segment.
*/
#define KERNEL_DS			(__KERNEL_DS * __SEGMENT_DESCR_SIZE)

/**
* \brief Setup segmentation support.
*
* Setup segmentation support. The function initializes Global Descriptior
*	Table and loads it to appropriate CPU register.
*/
void setup_segments();

#endif // __BOLGENOS_NG__MMU_H__
