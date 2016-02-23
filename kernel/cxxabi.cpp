#include <bolgenos-ng/error.h>

extern "C" void __cxa_pure_virtual()
{
	panic("Pure virtual function is called\n");
}

extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso) {
	(void) destructor;
	(void) arg;
	(void) dso;
	return 0;
}

extern "C" void *__dso_handle;
void *__dso_handle = nullptr;

void operator delete(void *dummy) {
	(void) dummy;
}
