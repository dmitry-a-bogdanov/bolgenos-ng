#include <bolgenos-ng/error.h>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/memory.hpp>
#include <lib/ostream.hpp>

#include <lib/atomic.hpp>

extern "C" {

typedef void (*ctor_function_t)();

_asm_linked_ char __ctors_begin[0];
_asm_linked_ char __ctors_end[0];


void call_global_ctors()
{
	char *begin = __ctors_begin;
	char *end = __ctors_end;
	for (; begin != end; begin += sizeof(char *)) {
		auto address = *reinterpret_cast<size_t*>(begin);
		auto f = reinterpret_cast<ctor_function_t>(address);
		f();
	}
}

void __cxa_pure_virtual()
{
	panic("Pure virtual function is called\n");
}

int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso) {
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

void *__dso_handle = (void *)(0x0);


} // extern "C"


void *operator new(size_t size)
{
	return memory::kmalloc(size);
}


void *operator new[](size_t size)
{
	return memory::kmalloc(size);
}


void operator delete(void *p) noexcept
{
	memory::kfree(p);
}


void operator delete[](void *p) noexcept
{
	memory::kfree(p);
}

