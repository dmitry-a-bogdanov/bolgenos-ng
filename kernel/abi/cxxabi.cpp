#include <unwind.h>

#include <atomic>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/memory.hpp>


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


typedef std::atomic_uchar __guard;
constexpr unsigned char INITIALIZATION_NOT_STARTED = 0x00;
constexpr unsigned char INITIALIZATION_STARTED = 0x01;
constexpr unsigned char INITIALIZATION_FINISHED = 0x03;


int __cxa_guard_acquire (__guard *g)
{
	if (*g == INITIALIZATION_FINISHED)
	{
		return 0;
	}

	unsigned char previous = INITIALIZATION_NOT_STARTED;
	if (g->compare_exchange(previous, INITIALIZATION_STARTED))
	{
		return 1;
	}

	while (*g == INITIALIZATION_STARTED) {}
	return 0;
}


void __cxa_guard_release (__guard *g)
{
	*g = INITIALIZATION_FINISHED;
}


void __cxa_guard_abort (__guard *g)
{
	*g = INITIALIZATION_FINISHED;
}


void *__dso_handle = nullptr;


}
// extern "C"
