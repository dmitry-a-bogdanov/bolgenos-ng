#include <bolgenos-ng/execinfo.hpp>

#include <bolgenos-ng/kernel_object.hpp>

#include <lib/ostream.hpp>

namespace {

struct  __attribute__((packed)) c_stack_frame {
	void* callers_ebp;
	void* return_address;
};

static_assert(sizeof(c_stack_frame) == 8, "Wrong size of structure\n");


bool is_inside_code(void *ptr) {
	return kobj::code_begin() <= ptr && ptr < kobj::code_end();
}


bool is_inside_stack(void *ptr) {
	return kobj::stack_begin() <= ptr && ptr < kobj::stack_end();
}


lib::ostream& operator <<(lib::ostream& out, const c_stack_frame& frame) {
	lib::scoped_format_guard format_guard(out);

	out	<< lib::hex;

	out 	<< "frame["
			<< const_cast<void *>(static_cast<const void *>(&frame))
			<< lib::setw(0)
		<< "]: ebp = "
		<< lib::setw(8);

	if (is_inside_stack(frame.callers_ebp)) {
		out	<< frame.callers_ebp;
	} else {
		out	<< "invalid";
	}

	out	<< lib::setw(0)
			<< " ret = "
		<< lib::setw(8);

	if (is_inside_code(frame.return_address)) {
		out	<< frame.return_address;
	} else {
		out	<< "invalid";
	}

	return out;
}


} // namespace


void execinfo::show_backtrace(lib::ostream& out,
		void *ebp, void *eip) {
	if (ebp == nullptr) {
		// Get pointer to begging of function's stack.
		// Caller's ebp and return instruction pointer are located in higher
		// adresses right after begging of the function's stack.
		asm ("mov %%ebp, %%eax\n": "=a"(ebp):);
	}


	c_stack_frame *frame;
	c_stack_frame tmp_frame;
	tmp_frame.callers_ebp = ebp;
	tmp_frame.return_address = eip;
	if (eip == nullptr) {
		frame = static_cast<c_stack_frame *>(ebp);
	} else {
		frame = &tmp_frame;
	}

	do {
		out << *frame << lib::endl;
		frame = static_cast<decltype(frame)>(frame->callers_ebp);
	} while (is_inside_stack(frame) &&
			is_inside_code(frame->return_address));
}

namespace {



} // namespace
