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

[[noreturn]] void raise_not_implemented(const char* msg = nullptr);

#define RAISE_NOT_IMPLEMENTED do { raise_not_implemented(__PRETTY_FUNCTION__); } while(false)

#ifdef __cplusplus
}
#endif


#endif // __BOLGENOS_NG__ERROR_H__
