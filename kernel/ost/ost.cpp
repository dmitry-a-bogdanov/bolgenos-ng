#include <bolgenos-ng/init_queue.hpp>
#include "memory.hpp"
#include "bitarray.hpp"

namespace
{

void run()
{
	ost::page_alloc_test();
	ost::free_list_test();
	ost::slab_test();
	ost::buddy_allocator_test();
	ost::test_bitarray();
	ost::mallocator_test();
}

using namespace bolgenos::init;

FunctorRegisterer registerer(prio_t::runtime_tests, "On start tests",
		[]() -> bool
		{
			run();
			return true;
		});

}
