#ifndef __BOLGENOS_NG__INT_TYPES_H__
#define __BOLGENOS_NG__INT_TYPES_H__


/**
* \file int_types.h
*
* It's not a problem to include stdint.h to bolgenos-ng kernel because
* this header is provided by cross-host compiler instead of host platform.
* However you have to NOT FORGET to pass -nostdlib flag to compiler.
*/
#include <stdint.h>


/**
* \brief Size type.
*
* Type that can hold at least number of bytes in RAM.
*/
typedef uint32_t size_t;

#endif // __BOLGENOS_NG__INT_TYPES_H__
