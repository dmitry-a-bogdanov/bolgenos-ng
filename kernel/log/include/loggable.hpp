#pragma once

#include <cstddef.hpp>
#include <ostream.hpp>
#include <string.hpp>
#include <memory.hpp>
#include "log.hpp"

namespace lib {

class Loggable {
public:
	Loggable(const Loggable&) = delete;
	Loggable& operator=(const Loggable&) = delete;

protected:
	Loggable(const char* logger_name, LogLevel level = LogLevel::NOTICE);

	~Loggable();


private:
	lib::LogLevel _log_level;

	struct Streambufs;
	lib::unique_ptr<Streambufs> _streambufs;

protected:
	lib::ostream INFO;
	lib::ostream NOTICE;
	lib::ostream WARN;
	lib::ostream ERROR;
	lib::ostream CRIT;
};

}
