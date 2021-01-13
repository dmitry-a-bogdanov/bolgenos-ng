#pragma once

#include <mutex.hpp>

namespace thr {


class RecursiveIrqLocker {
public:
	void lock();
	void unlock();
private:
	bool _enable_on_unlock{};
};


class RecursiveIrqGuard {
public:
	RecursiveIrqGuard();
	~RecursiveIrqGuard();
private:
	RecursiveIrqLocker _lock;
	lib::lock_guard<RecursiveIrqLocker> _lock_guard;
};

}
