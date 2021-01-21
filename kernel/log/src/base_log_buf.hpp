#pragma once

#include <concepts.hpp>
#include <log.hpp>
#include <utility.hpp>
#include <streambuf.hpp>

#include "simple_stream_buf.hpp"

template<lib::derived_from<lib::streambuf> BaseBuf>
class BaseLogBuf: public SimpleStreamBuf<BaseBuf> {
public:
	BaseLogBuf() = delete;
	BaseLogBuf(BaseLogBuf&&) = default;
	BaseLogBuf(const BaseLogBuf&) = delete;
	BaseLogBuf& operator=(const BaseLogBuf&) = delete;

	template<class ...Args>
	BaseLogBuf(lib::LogLevel log_level, const char* prefix, lib::LogLevel& enabled_log_level, Args ...args)
		: SimpleStreamBuf<BaseBuf>(lib::forward<Args>(args)...),
		        _log_level{log_level}, _prefix{prefix}, _enabled_log_level(enabled_log_level)
	{}

	~BaseLogBuf() override = default;
protected:
	void do_start_log_line() {
		handle_pre_prefix();
		auto prefix = _prefix;
		while (*prefix != '\0') {
			SimpleStreamBuf<BaseBuf>::overflow(*(prefix++));
		}
		handle_post_prefix();
	}

	void do_end_long_line() {
		handle_end_line();
	}

	virtual void handle_pre_prefix() {}
	virtual void handle_post_prefix() {}
	virtual void handle_end_line() {}

	int overflow(int c) final {
		if (_log_level > _enabled_log_level)
			return c;
		if (_show_header) {
			do_start_log_line();
			_show_header = false;
		}
		int result = SimpleStreamBuf<BaseBuf>::overflow(c);
		if (c == '\n') {
			do_end_long_line();
			_show_header = true;
		}
		return result;
	}

private:
	lib::LogLevel _log_level;
	const char *_prefix;
	bool _show_header{true};
	lib::LogLevel& _enabled_log_level;
};
