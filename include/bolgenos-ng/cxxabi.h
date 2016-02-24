#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void call_global_ctors();

void __cxa_pure_virtual();

int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso);

#ifdef __cplusplus
}
#endif
