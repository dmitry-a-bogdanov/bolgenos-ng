#ifndef __BOLGENOS_NG__ERROR_H__
#define __BOLGENOS_NG__ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Print critical error and hang forever.
*/
void panic(const char *msg) __attribute__((noreturn));


/**
* \brief Print non-critical error.
*/
void bug(const char *msg) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif // __BOLGENOS_NG__ERROR_H__
