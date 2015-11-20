#ifndef __BOLGENOS_NG__INT_TYPES_H__
#define __BOLGENOS_NG__INT_TYPES_H__ (1)

/*
 * It's not a problem to include stdint.h to Bolgenos-NG kernel because
 * this header is provided by cross-host compiler instead of host platform.
 * However you have to NOT FORGET to set -nostdlib flag
 */
#include <stdint.h>

typedef uint32_t size_t;

#endif // __BOLGENOS_NG__INT_TYPES_H__
