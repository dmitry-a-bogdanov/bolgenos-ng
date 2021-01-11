#pragma once

#include <mutex.hpp>

namespace thr {

template<typename MutexType, typename F>
auto with_lock(F&& f) {
	MutexType mutex;
	lib::lock_guard guard{mutex};
	return f();
}

}