#pragma once

#include <mutex.hpp>
#include <utility.hpp>

#include "lock.hpp"

namespace thr {

template<typename MutexType, typename F>
inline auto with_lock(F&& f) {
	MutexType mutex;
	lib::lock_guard guard{mutex};
	return f();
}

template<typename F>
inline auto with_irq_lock(F&& f) {
	return with_lock<RecursiveIrqLocker>(lib::forward<F>(f));
}

}