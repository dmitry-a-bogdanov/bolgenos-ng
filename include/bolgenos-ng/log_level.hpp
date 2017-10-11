#pragma once

#include <ostream>

namespace lib
{

enum log_level_type: int
{
	critical,
	error,
	warning,
	notice,
	info,
	debug,
};

log_level_type get_log_level();
void set_log_level(log_level_type log_level);

extern std::ostream ccrit;
extern std::ostream cerr;
extern std::ostream cwarn;
extern std::ostream cnotice;
extern std::ostream cinfo;
extern std::ostream cdebug;

} // namespace lib

