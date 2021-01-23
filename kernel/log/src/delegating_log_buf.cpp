#include "log/delegating_log_buf.hpp"

int log::DelegatingLogBuf::overflow(int c) {
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

void log::DelegatingLogBuf::do_start_log_line()
{
	handle_pre_prefix();
	auto prefix = _prefix;
	while (*prefix != '\0') {
		if (_delegate) _delegate->sputc(*(prefix++));
	}
	handle_post_prefix();
}

void log::DelegatingLogBuf::do_end_long_line()
{
	if (_delegate) handle_end_line();
}

