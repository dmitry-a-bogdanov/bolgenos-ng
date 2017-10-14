#include <bolgenos-ng/init_queue.hpp>
#include <bolgenos-ng/ost/test_runner.hpp>

namespace
{

void run()
{
	ost::TestRunner::default_runner()->execute();
}

using namespace bolgenos::init;

FunctorRegisterer registerer(prio_t::runtime_tests, "On start tests",
		[]() -> bool
		{
			run();
			return true;
		});

}
