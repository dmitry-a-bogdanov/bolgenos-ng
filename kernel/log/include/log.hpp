#pragma once

#include <ostream.hpp>

namespace dev {
class SerialPort;
}

namespace lib {


/// Logging level type.
enum class LogLevel: int {
	CRITICAL	= 0,
	ERROR		= 1,
	WARNING		= 2,
	NOTICE		= 3,
	INFO		= 4,
};


/// Get current system wide logging level.
LogLevel get_log_level();


/// Set new system wide logging level.
void set_log_level(LogLevel log_level);

void add_serial(dev::SerialPort&& serial_port);

/// Output object for critical error messages.
extern ostream ccrit;


/// Output object for error messages.
extern ostream cerr;


/// Output object for warning messages.
extern ostream cwarn;


/// Output object for notice messages.
extern ostream cnotice;


/// Output object for info messages.
extern ostream cinfo;

}