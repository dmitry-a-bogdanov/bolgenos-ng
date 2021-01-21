#include <loggable.hpp>

#include <log.hpp>

#include "vga_log_buf.hpp"

using namespace vga_console;

struct lib::Loggable::Streambufs {
private:
	string _prefix;
public:

	Streambufs(string prefix, LogLevel& config):
		_prefix(lib::move(prefix)),
		info{LogLevel::INFO, _prefix.c_str(), config, color_t::bright_green},
		notice{LogLevel::NOTICE, _prefix.c_str(), config, color_t::green},
		warn{LogLevel::WARNING, _prefix.c_str(), config, color_t::yellow},
		error{LogLevel::ERROR, _prefix.c_str(), config, color_t::bright_red},
		crit{LogLevel::CRITICAL, _prefix.c_str(), config, color_t::red}
	{}

	VgaLogBuf info;
	VgaLogBuf notice;
	VgaLogBuf warn;
	VgaLogBuf error;
	VgaLogBuf crit;
};

lib::Loggable::Loggable(const char* logger_name, LogLevel level):
	_log_level{level},
	_streambufs{lib::make_unique<Streambufs>(logger_name + string{": "}, _log_level)},
	INFO{&_streambufs->info},
	NOTICE{&_streambufs->notice},
	WARN{&_streambufs->warn},
	ERROR{&_streambufs->error},
	CRIT{&_streambufs->crit}
{
}

lib::Loggable::~Loggable() = default;
