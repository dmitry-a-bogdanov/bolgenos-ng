#include <bolgenos-ng/init_queue.hpp>

#include <bolgenos-ng/irq.hpp>

using namespace bolgenos::init;

FunctorRegisterer start_serving_irq(prio_t::start_serving_irq, "Start serving IRQ",
		[]() -> bool
		{
			irq::enable();
			return true;
		});


Queue& Queue::instance()
{
	static Queue queue;
	return queue;
}


void Queue::execute() const
{
	lib::cnotice << "Starting kernel init queues" << lib::endl;
	for(auto& routines_on_level: _routines)
	{
		if (routines_on_level.empty())
		{
			continue;
		}

		auto level = &routines_on_level - _routines;
		lib::cnotice << "| queue[" << level << "]:" << lib::endl;
		for(auto& routine: routines_on_level)
		{
			lib::cnotice << "|  -> " << routine->name() << lib::endl;
			if (!routine->invoke())
			{
				lib::cerr << "initialization routine '" << routine->name() << "' failed" << lib::endl;
				panic("Failed to start kernel");
			}
		}
	}
	lib::cnotice << "Finished kernel init queues" << lib::endl;
}


void Queue::register_callback(node_type& node, prio_t prio)
{
	_routines[basalt::to_integral(prio)].push_front(node);
}




