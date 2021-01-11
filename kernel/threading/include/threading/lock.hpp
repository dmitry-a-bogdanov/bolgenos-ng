#pragma once

namespace thr {


class RecursiveIrqLocker {
public:
	void lock();
	void unlock();
private:
	bool _enable_on_unlock{};
};


}
