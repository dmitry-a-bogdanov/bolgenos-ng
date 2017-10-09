#include <bolgenos-ng/init_queue.hpp>

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/log.hpp>

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
	LOG_NOTICE("Starting kernel init queues");
	for(auto& routines_on_level: _routines)
	{
		if (routines_on_level.empty())
		{
			continue;
		}

		auto level = &routines_on_level - _routines;
		LOG_NOTICE("| queue[" << level << "]:");
		for(auto& routine: routines_on_level)
		{
			LOG_NOTICE("|  -> " << routine->name());
			if (!routine->invoke())
			{
				LOG_CRITICAL("initialization routine '"
					<< routine->name() << "' failed");
				panic("Failed to start kernel");
			}
		}
	}
	LOG_NOTICE("Finished kernel init queues");
}


void Queue::register_callback(node_type& node, prio_t prio)
{
	_routines[basalt::to_integral(prio)].push_front(node);
}




