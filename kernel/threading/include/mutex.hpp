#pragma once

namespace lib {

template<class Mutex>
class lock_guard {
public:
	using mutex_type = Mutex;

	explicit lock_guard(mutex_type& m)
		: _mutex{m}
	{
		_mutex.lock();
	}

	// TBD
	// lock_guard(mutex_type& m, adopt_lock_t);

	~lock_guard() {
		_mutex.unlock();
	}

	lock_guard(const lock_guard&) = delete;
	lock_guard& operator=(const lock_guard&) = delete;
private:
	mutex_type& _mutex;
};

}