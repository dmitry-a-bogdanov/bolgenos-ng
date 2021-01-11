#include <threading/lock.hpp>

#include <bolgenos-ng/irq.hpp>

void thr::RecursiveIrqLocker::lock()
{
	_enable_on_unlock = irq::disable(false);
}

void thr::RecursiveIrqLocker::unlock()
{
	if (_enable_on_unlock) {
		irq::enable();
	}
}
