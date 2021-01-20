#include <execinfo.hpp>

#include <bolgenos-ng/kernel_object.hpp>

#include <ostream.hpp>
#include <ext/scoped_format_guard.hpp>

namespace {


struct  __attribute__((packed)) stack_frame_t {
	void* callers_ebp;
	void* return_address;
};

static_assert(sizeof(stack_frame_t) == 8, "Wrong size of structure\n");


bool is_inside_code(void *ptr) {
	return kobj::code_begin() <= ptr && ptr < kobj::code_end();
}


bool is_inside_stack(void *ptr) {
	return kobj::stack_begin() <= ptr && ptr < kobj::stack_end();
}


lib::ostream& operator <<(lib::ostream& out, const stack_frame_t& frame) {
	lib::ScopedFormatGuard format_guard(out);

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
		<< " eip = "
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
	lib::ScopedFormatGuard format_guard(out);

	out	<< lib::setw(50) << lib::setfill('-')
		<< '\n'
		<< lib::setfill(' ') << lib::setw(0);

	if (ebp == nullptr) {
		// Get pointer to begging of function's stack.
		// Caller's ebp and return instruction pointer are located in higher
		// adresses right after begging of the function's stack.
		asm ("mov %%ebp, %%eax\n": "=a"(ebp):);
	}


	stack_frame_t *frame;
	stack_frame_t tmp_frame;
	tmp_frame.callers_ebp = ebp;
	tmp_frame.return_address = eip;
	if (eip == nullptr) {
		frame = static_cast<stack_frame_t *>(ebp);
	} else {
		frame = &tmp_frame;
	}

	do {
		out << *frame << lib::endl;
		auto new_frame = static_cast<decltype(frame)>(frame->callers_ebp);
		if (frame == new_frame) {
			break;
		} else {
			frame = new_frame;
		}
	} while (is_inside_stack(frame) &&
			is_inside_code(frame->return_address));
}


namespace {



} // namespace
