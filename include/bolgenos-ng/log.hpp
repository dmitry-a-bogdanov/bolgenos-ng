#pragma once

#include <lib/ostream.hpp>
#include <basalt/format_guard.hpp>

#define _LOG_STREAM(stream, expr) \
	do { \
		basalt::scoped_format_guard fguard(stream); \
		stream << expr << lib::endl; \
	} while(false)

#define LOG_CRITICAL(expr) _LOG_STREAM(lib::ccrit, expr)
#define LOG_ERROR(expr) _LOG_STREAM(lib::cerr, expr)
#define LOG_WARNING(expr) _LOG_STREAM(lib::cwarn, expr)
#define LOG_NOTICE(expr) _LOG_STREAM(lib::cnotice, expr)
#define LOG_INFO(expr) _LOG_STREAM(lib::cinfo, expr)
#define LOG_DEBUG(expr) _LOG_STREAM(lib::cdebug, expr)

