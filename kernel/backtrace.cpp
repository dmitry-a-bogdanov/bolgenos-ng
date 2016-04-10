#include <bolgenos-ng/execinfo.hpp>


#include <bolgenos-ng/cout.hpp>

namespace {

struct  __attribute__((packed)) c_stack_frame {
	void* callers_ebp;
	void* return_address;
};

static_assert(sizeof(c_stack_frame) == 8, "Wrong size of structure\n");

} // namespace

void test_backtrace3(int arg __attribute__((unused))) {
	cio::cout << __func__ << ":" << reinterpret_cast<void *>(test_backtrace3) << cio::endl;
	execinfo::show_backtrace(nullptr);
}

void execinfo::show_backtrace(void *base_pointer) {
	if (base_pointer == nullptr) {
		// Get pointer to begging of function's stack.
		// Caller's ebp and return instruction pointer are located in higher
		// adresses right after begging of the function's stack.
		asm ("mov %%ebp, %%eax\n": "=a"(base_pointer):);
	}
	cio::cout << "dumping from stack from " << base_pointer << cio::endl;

	auto frame = reinterpret_cast<c_stack_frame *>(base_pointer);


	cio::cout << "frame[" << frame << "]: ebp = " << frame->callers_ebp << " ret = " << frame->return_address << cio::endl;
	frame = reinterpret_cast<decltype(frame)>(frame->callers_ebp);
	cio::cout << "frame[" << frame << "]: ebp = " << frame->callers_ebp << " ret = " << frame->return_address << cio::endl;
	frame = reinterpret_cast<decltype(frame)>(frame->callers_ebp);
	cio::cout << "frame[" << frame << "]: ebp = " << frame->callers_ebp << " ret = " << frame->return_address << cio::endl;
	frame = reinterpret_cast<decltype(frame)>(frame->callers_ebp);
	cio::cout << "frame[" << frame << "]: ebp = " << frame->callers_ebp << " ret = " << frame->return_address << cio::endl;
	frame = reinterpret_cast<decltype(frame)>(frame->callers_ebp);
	cio::cout << "frame[" << frame << "]: ebp = " << frame->callers_ebp << " ret = " << frame->return_address << cio::endl;
}

namespace {



} // namespace
