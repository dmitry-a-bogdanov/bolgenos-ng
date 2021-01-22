#pragma once

#include <ostream.hpp>

#include "log_level.hpp"

namespace serial {
class SerialPort;
}

namespace lib {


/// Get current system wide logging level.
LogLevel get_log_level();


/// Set new system wide logging level.
void set_log_level(LogLevel log_level);

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