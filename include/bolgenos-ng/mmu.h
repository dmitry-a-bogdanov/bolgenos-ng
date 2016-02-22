#ifndef __BOLGENOS_NG__MMU_H__
#define __BOLGENOS_NG__MMU_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
* Size of segment descriptor.
*/
#define STRUCT_SEG_SIZE			(8)


/**
* Index of null segment.
*/
#define __NULL_SEGMENT			(0x0)


/**
* Null segment.
*/
#define NULL_SEGMENT			(__NULL_SEGMENT * STRUCT_SEG_SIZE)


/**
* Index of kernel code segment.
*/
#define __KERNEL_CS			(1)


/**
* Kernel code segment.
*/
#define KERNEL_CS			(__KERNEL_CS * STRUCT_SEG_SIZE)


/**
* Index of kernel data segment.
*/
#define __KERNEL_DS			(2)


/**
* Kernel data segment.
*/
#define KERNEL_DS			(__KERNEL_DS * STRUCT_SEG_SIZE)


/**
* \brief Setup segmentation support.
*
* Setup segmentation support. The function initializes Global Descriptior
*	Table and loads it to appropriate CPU register.
*/
void setup_segments();

#ifdef __cplusplus
}
#endif

#endif // __BOLGENOS_NG__MMU_H__
