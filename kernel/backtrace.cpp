#include <bolgenos-ng/execinfo.hpp>

#include <ostream>

#include <bolgenos-ng/kernel_object.hpp>

#include <basalt/format_guard.hpp>

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


std::ostream& operator <<(std::ostream& out, const stack_frame_t& frame) {
	basalt::scoped_format_guard format_guard(out);

	out	<< std::hex;

	out 	<< "frame["
			<< const_cast<void *>(static_cast<const void *>(&frame))
			<< std::setw(0)
		<< "]: ebp = "
		<< std::setw(8);

	if (is_inside_stack(frame.callers_ebp)) {
		out	<< frame.callers_ebp;
	} else {
		out	<< "invalid";
	}

	out	<< std::setw(0)
		<< " eip = "
		<< std::setw(8);

	if (is_inside_code(frame.return_address)) {
		out	<< frame.return_address;
	} else {
		out	<< "invalid";
	}

	return out;
}


} // namespace


void execinfo::show_backtrace(std::ostream& out,
		void *ebp, void *eip) {
	basalt::scoped_format_guard format_guard(out);

	out	<< std::setw(50) << std::setfill('-')
		<< '\n'
		<< std::setfill(' ') << std::setw(0);

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
		out << *frame << std::endl;
		frame = static_cast<decltype(frame)>(frame->callers_ebp);
	} while (is_inside_stack(frame) &&
			is_inside_code(frame->return_address));
}

namespace {



} // namespace
