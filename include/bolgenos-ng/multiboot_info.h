#ifndef __BOLGENOS_NG__MULTIBOOT_INFO_H__
#define __BOLGENOS_NG__MULTIBOOT_INFO_H__

#include <bolgenos-ng/int_types.h>


/**
* \brief Read multiboot info.
*
* Copy boot information structure provided by Multiboot-compliant bootloader to
* internal kernel memory.
*/
void multiboot_info_init();


/**
* \brief Check that memory information is valid.
*
* Check that multiboot-compliant bootloader provided valid information about
* memory on this machine.
*
* \warning Calling this function before \ref multiboot_info_init will result
* in undefined behavior.
*
* \return
* 1 if memory info is valid;
* 0 otherwise.
*/
int mboot_is_meminfo_valid();


/**
* \brief Get size of low memory.
*
* Get size of low memory (located lower 1M) provided by multiboot-compliant
* bootloader.
*
* \warning Calling this function before \ref multiboot_info_init will result
* in undefined behavior.
*
* \return Size of low memory.
*/
uint32_t mboot_get_low_mem();


/**
* \brief Get size of high memory.
*
* Get size of high memory (located upper 1M) provided by multiboot-compliant
* bootloader.
*
* \warning Calling this function before \ref multiboot_info_init will result
* in undefined behavior.
*
* \return Size of high memory.
*/
uint32_t mboot_get_high_mem();

#endif // __BOLGENOS_NG__MULTIBOOT_H__
