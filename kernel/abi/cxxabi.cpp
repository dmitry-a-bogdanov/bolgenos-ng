#include <unwind.h>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/memory.hpp>
#include <lib/ostream.hpp>

#include <lib/atomic.hpp>


extern "C"
{


using ctor_function_t = void ();


_asm_linked_ ctor_function_t* __ctors_begin[];
_asm_linked_ ctor_function_t* __ctors_end[];


void call_global_ctors()
{
    for (auto begin = __ctors_begin; begin != __ctors_end; ++begin)
    {
        (*begin)();
    }
}


void __cxa_pure_virtual()
{
	panic("Pure virtual function is called\n");
}


int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso)
{
	(void) destructor;
	(void) arg;
	(void) dso;
	return 0;
}


typedef lib::atomic<bool> __guard;


int __cxa_guard_acquire (__guard *g)
{
	if (g->compare_exchange(false, true)) {
		return 1; // just to make sure that true is casted to 1
	} else {
		return 0;
	}
}


void __cxa_guard_release (__guard *g __attribute__((unused)))
{
}


void __cxa_guard_abort (__guard *g)
{
	g->compare_exchange(true, false);
}


void *__dso_handle = nullptr;


}
// extern "C"
