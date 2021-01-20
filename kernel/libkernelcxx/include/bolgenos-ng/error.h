#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Print critical error and hang forever.
*/
[[noreturn]]
void panic(const char *msg = nullptr);


/**
* \brief Print non-critical error.
*/
[[noreturn]]
void bug(const char *msg);

[[noreturn]]
void raise_not_implemented(const char* msg = nullptr);

#define RAISE_NOT_IMPLEMENTED do { raise_not_implemented(__PRETTY_FUNCTION__); } while(false)

#ifdef __cplusplus
}
#endif
