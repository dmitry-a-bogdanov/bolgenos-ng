#pragma once

#include <ostream.hpp>

namespace lib {


/// Logging level type.
enum log_level_type: int {
	/// Critical error
	critical	= 0,
	/// Error
	error		= 1,
	/// Warning
	warning		= 2,
	/// Notice
	notice		= 3,
	/// Additional information
	info		= 4,
};


/// Get current system wide logging level.
log_level_type get_log_level();


/// Set new system wide logging level.
void set_log_level(log_level_type log_level);


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