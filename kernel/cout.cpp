#include <bolgenos-ng/cout.hpp>

#include <bolgenos-ng/printk.h>


cio::OutStream cio::cout;


cio::OutStream& cio::OutStream::operator <<(const char *string) {
	printk("%s", string);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(void *ptr) {
	printk("%lu", (long unsigned) ptr);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(signed long val) {
	printk("%li", val);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(signed int val) {
	return *this << (signed long) val;
}


cio::OutStream& cio::OutStream::operator <<(unsigned long val) {
	printk("%lu", val);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(unsigned int val) {
	return *this << (unsigned long) val;
}


cio::OutStream& cio::OutStream::operator <<(format_func_type func) {
	return func(*this);
}


cio::OutStream& cio::endl(OutStream &stream) {
	stream << "\n";
	return stream;
}
