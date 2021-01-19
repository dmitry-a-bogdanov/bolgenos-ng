#pragma once


#ifdef __cplusplus
extern "C" {
#endif


/// \brief Call global constructors.
///
/// The function calls constuctors of global objects by calling pointers
/// which are stored in `.ctors` ELF-section. The function is required by
/// C++ ABI.
void call_global_ctors();


/// \brief Pure virtual function.
///
/// Function that will be called if pure virtual function call happens. The
/// function calls panic. The function is required by C++ ABI.
void __cxa_pure_virtual();


/// \brief Register function to call at exit.
///
/// Empty implementation of __cxa_atexit function that is needed for C++ ABI.
int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso);


#ifdef __cplusplus
}
#endif
