#pragma once

#include <cstddef.hpp>
#include <ostream.hpp>
#include <static_string.hpp>
#include <string.hpp>
#include <memory.hpp>
#include "log_level.hpp"
#include "logger.hpp"

namespace lib {

// in C++20 this template can depend on non-type parameter like
// `template<lib::basic_static_string LogPrefix> class StaticLoggable`
// however due to bug in gcc-10 nested types inside the template cannot be deduced.
// the problem is fixed in gcc-11 but it hasn't been released yet.
// FIXME: remove specialization with gcc-11
template<class Char, char ...Chars>
class StaticLoggable;

template<class Char, char ...Chars>
class StaticLoggable<lib::basic_static_string<Char, Chars...>> {
private:
	using prefix = lib::basic_static_string<Char, Chars...>;
	static ::lib::StaticLogger<prefix> _local_logger;
protected:
	static auto& DEBUG = _local_logger.debug();
	static auto& INFO = _local_logger.info();
	static auto& NOTICE = _local_logger.notice();
	static auto& WARN = _local_logger.warning();
	static auto& ERROR = _local_logger.error();
	static auto& CRIT = _local_logger.critical();
};

template<class Char, char ...Chars>
lib::StaticLogger<typename StaticLoggable<lib::basic_static_string<Char, Chars...>>::prefix>
StaticLoggable<lib::basic_static_string<Char, Chars...>>::_local_logger{{}, lib::LogLevel::NOTICE};

#define Loggable(prefix) ::lib::StaticLoggable<decltype(prefix ## _ss + ": "_ss)>

}
