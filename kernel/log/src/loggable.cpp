#include <loggable.hpp>

#include <log.hpp>
#include <string.hpp>

#include <bolgenos-ng/vga_console.hpp>

#include "vga_log_buf.hpp"
#include "buf_builder.hpp"

using namespace lib;
using namespace vga_console;

struct lib::Loggable::Streambufs {
private:
	lib::string _prefix;
public:
	Streambufs() = delete;

	Streambufs(string prefix, LogLevel& config)
		: _prefix(lib::move(prefix))
	{
		const char* p = _prefix.c_str();
		info = log::build_global_buf(LogLevel::INFO, p, config);
		notice = log::build_global_buf(LogLevel::NOTICE, p, config);
		warn = log::build_global_buf(LogLevel::WARNING, p, config);
		error = log::build_global_buf(LogLevel::ERROR, p, config);
		crit = log::build_global_buf(LogLevel::CRITICAL, p, config);
	}

	using streambuf_type = CompositeBuf<log::VgaDelegatingLogBuf, log::SerialDelegatingLogBuf>;
	streambuf_type info{};
	streambuf_type notice{};
	streambuf_type warn{};
	streambuf_type error{};
	streambuf_type crit{};
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
