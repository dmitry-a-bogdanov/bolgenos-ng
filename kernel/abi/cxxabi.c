#include <bolgenos-ng/error.h>
#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/printk.h>

typedef void (*ctor_function_t)();

_asm_linked_ char __ctors_begin[0];
_asm_linked_ char __ctors_end[0];

void call_global_ctors() {
	char *begin = __ctors_begin;
	char *end = __ctors_end;
	for (; begin != end; begin += sizeof(char *)) {
		ctor_function_t f = (void *) *((unsigned long*) begin);
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

void *__dso_handle = (void *)(0x0);


