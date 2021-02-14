#pragma once

#include <streambuf.hpp>
#include <static_string.hpp>
#include "simple_stream_buf.hpp"

#include <log_level.hpp>

namespace log {

template<class Char, Char ...chars>
class StaticDelegatingLogBuf;

template<class Char, Char ...chars>
class StaticDelegatingLogBuf<lib::basic_static_string<Char, chars...>>: public SimpleStreamBuf<lib::streambuf> {
	using prefix = lib::basic_static_string<Char, chars...>;
public:
	StaticDelegatingLogBuf() = delete;
	StaticDelegatingLogBuf(StaticDelegatingLogBuf&&) = default;
	StaticDelegatingLogBuf(const StaticDelegatingLogBuf&) = delete;
	StaticDelegatingLogBuf& operator=(const StaticDelegatingLogBuf&) = delete;

	StaticDelegatingLogBuf(lib::LogLevel log_level, const prefix&, lib::LogLevel& enabled_log_level, lib::streambuf* delegate = nullptr)
		: _log_level{log_level}, _enabled_log_level(enabled_log_level), _delegate(delegate)
	{}

	lib::streambuf* delegate() const { return _delegate; }
	lib::streambuf* delegate(lib::streambuf* delegate) { return lib::exchange(_delegate, delegate); }

	~StaticDelegatingLogBuf() override = default;
protected:
	void do_start_log_line() {
		handle_pre_prefix();
		if (_delegate) {
			(_delegate->sputc(chars), ...);
		}
		handle_post_prefix();
	}

	void do_end_long_line()
	{
		if (_delegate) handle_end_line();
	}


	virtual void handle_pre_prefix() {}
	virtual void handle_post_prefix() {}
	virtual void handle_end_line() {}

	int overflow(int c) final {
		if (_log_level > _enabled_log_level)
			return c;
		if (_show_header) {
			if (_delegate) do_start_log_line();
			_show_header = false;
		}
		int result = c;
		if (_delegate) {
			result = _delegate->sputc(c);
		}
		if (c == '\n') {
			if (_delegate) do_end_long_line();
			_show_header = true;
		}
		return result;
	}

private:
	lib::LogLevel _log_level;
	bool _show_header{true};
	lib::LogLevel& _enabled_log_level;

	lib::streambuf* _delegate{nullptr};
};


}
