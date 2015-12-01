#ifndef __BOLGENOS_NG__ERROR_H__
#define __BOLGENOS_NG__ERROR_H__ (1)

void panic(const char *msg) __attribute__((noreturn));
void bug(const char *msg) __attribute__((noreturn));


#endif // __BOLGENOS_NG__ERROR_H__
