#include <bolgenos-ng/execinfo.hpp>

#include <bolgenos-ng/kernel_object.hpp>

#include <lib/ostream.hpp>

namespace {

struct  __attribute__((packed)) c_stack_frame {
	void* callers_ebp;
	void* return_address;
};

static_assert(sizeof(c_stack_frame) == 8, "Wrong size of structure\n");


lib::ostream& operator <<(lib::ostream& out, const c_stack_frame& frame) {
	out 	<< "frame[";
	auto oldwidth = out.width(8);
	auto oldflags = out.setf(lib::ostream::fmtflags::hex,
			lib::ostream::fmtflags::basefield);
	out 	<< const_cast<void *>(static_cast<const void *>(&frame));
	out 	<< lib::setw(0);
	out 	<< "]: ebp = ";
	out 	<< lib::setw(8)
			<< frame.callers_ebp
		<< lib::setw(0)
		<< " ret = "
		<< lib::setw(8)
			<< frame.return_address;
	out.setf(oldflags, lib::ostream::fmtflags::basefield);
	out.width(oldwidth);
	return out;
}


bool is_inside_code(void *ptr) {
	return kobj::code_begin() <= ptr && ptr < kobj::code_end();
}


bool is_inside_stack(void *ptr) {
	return kobj::stack_begin() <= ptr && ptr < kobj::stack_end();
}


} // namespace


void test_backtrace3(int arg __attribute__((unused))) {
	lib::cout << __func__ << ":" << reinterpret_cast<void *>(test_backtrace3) << lib::endl;
	execinfo::show_backtrace(nullptr);
}


void execinfo::show_backtrace(void *base_pointer) {
	if (base_pointer == nullptr) {
		// Get pointer to begging of function's stack.
		// Caller's ebp and return instruction pointer are located in higher
		// adresses right after begging of the function's stack.
		asm ("mov %%ebp, %%eax\n": "=a"(base_pointer):);
	}
	lib::cout << "dumping from stack from "
		<< lib::hex << base_pointer << lib::dec << lib::endl;


	auto frame = static_cast<c_stack_frame *>(base_pointer);
	while (is_inside_stack(frame) &&
			is_inside_code(frame->return_address)) {
		lib::cout << *frame << lib::endl;
		frame = static_cast<decltype(frame)>(frame->callers_ebp);
	}
}

namespace {



} // namespace
