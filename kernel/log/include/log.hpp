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

}