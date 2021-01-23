#pragma once

#include <streambuf.hpp>
#include "simple_stream_buf.hpp"

#include <log_level.hpp>

namespace log {


class DelegatingLogBuf: public SimpleStreamBuf<lib::streambuf> {
public:
	DelegatingLogBuf() = delete;
	DelegatingLogBuf(DelegatingLogBuf&&) = default;
	DelegatingLogBuf(const DelegatingLogBuf&) = delete;
	DelegatingLogBuf& operator=(const DelegatingLogBuf&) = delete;

	DelegatingLogBuf(lib::LogLevel log_level, const char* prefix, lib::LogLevel& enabled_log_level, lib::streambuf* delegate = nullptr)
		: _log_level{log_level}, _prefix{prefix}, _enabled_log_level(enabled_log_level), _delegate(delegate)
	{}

	lib::streambuf* delegate() const { return _delegate; }
	lib::streambuf* delegate(lib::streambuf* delegate) { return lib::exchange(_delegate, delegate); }

	~DelegatingLogBuf() override = default;
protected:
	void do_start_log_line();
	void do_end_long_line();

	virtual void handle_pre_prefix() {}
	virtual void handle_post_prefix() {}
	virtual void handle_end_line() {}

	int overflow(int c) final;

private:
	lib::LogLevel _log_level;
	const char *_prefix;
	bool _show_header{true};
	lib::LogLevel& _enabled_log_level;

	lib::streambuf* _delegate{nullptr};
};

}
