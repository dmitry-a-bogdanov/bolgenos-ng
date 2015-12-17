#ifndef __BOLGENOS_NG__BOOTSTRAP_H__
#define __BOLGENOS_NG__BOOTSTRAP_H__

/**
* \showinitializer
* \brief Default kernel stack size.
*
* Macro defines default size of kernel bootstrap stack in bytes. This value
* will be used if the buildsystem doesn't provide custom value.
*/
#define DEFAULT_KERNEL_STACK_SIZE (16*1024)

/**
* \def KERNEL_STACK_SIZE
* \brief Kernel stack size.
*
* Macro defines size of kernel bootstrap stack in bytes. If the buildsystem
* doesn't set CONFIG_KERNEL_STACK_SIZE, value of \ref DEFAULT_KERNEL_STACK_SIZE
* will be used.
*/
#if defined(CONFIG_KERNEL_STACK_SIZE)
#	define KERNEL_STACK_SIZE (CONFIG_KERNEL_STACK_SIZE)
#else
#	define KERNEL_STACK_SIZE (DEFAULT_KERNEL_STACK_SIZE)
#endif

#endif // __BOLGENOS_NG__BOOTSTRAP_H__
