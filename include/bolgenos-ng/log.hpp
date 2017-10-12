#pragma once

#include <ostream>
#include <basalt/format_guard.hpp>
#include <basalt/scope_logger.hpp>

#define _LOG_STREAM(stream, expr) \
	do { \
		basalt::scoped_format_guard fguard(stream); \
		stream << expr << std::endl; \
	} while(false)

#define _LOG_F_STREAM(stream, expr) \
	_LOG_STREAM(stream, __func__ << ": " << expr)

#define LOG_CRITICAL(expr) _LOG_STREAM(lib::ccrit, expr)
#define LOG_F_CRITICAL(expr) _LOG_F_STREAM(lib::ccrit, expr)
#define LOG_ERROR(expr) _LOG_STREAM(lib::cerr, expr)
#define LOG_WARNING(expr) _LOG_STREAM(lib::cwarn, expr)
#define LOG_NOTICE(expr) _LOG_STREAM(lib::cnotice, expr)
#define LOG_INFO(expr) _LOG_STREAM(lib::cinfo, expr)
#define LOG_F_INFO(expr) _LOG_F_STREAM(lib::cinfo, expr)
#define LOG_DEBUG(expr) _LOG_STREAM(lib::cdebug, expr)

#define DEBUG_FUNCTION() \
	basalt::ScopeLogger ScopeLogger_##__LINE__ (lib::cdebug, __func__)

namespace lib
{

extern std::ostream ccrit;
extern std::ostream cerr;
extern std::ostream cwarn;
extern std::ostream cnotice;
extern std::ostream cinfo;
extern std::ostream cdebug;

}
