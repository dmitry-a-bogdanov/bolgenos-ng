#pragma once

#include <streambuf.hpp>
#include <utility.hpp>
#include "simple_stream_buf.hpp"
#include "delegating_log_buf.hpp"
#include "serial_buf.hpp"
#include <static_string.hpp>
#include "vga_buf.hpp"

namespace log {

template<class Char, Char ...Chars>
class StaticSerialLogBuf;

template<class Char, Char ...Chars>
class StaticSerialLogBuf<lib::basic_static_string<Char, Chars...>>
        : public StaticDelegatingLogBuf<lib::basic_static_string<Char, Chars...>> {
private:
	using prefix = lib::basic_static_string<Char, Chars...>;
public:
	StaticSerialLogBuf(lib::LogLevel log_level, const prefix&, lib::LogLevel& enabled_log_level)
		: StaticDelegatingLogBuf<prefix>{log_level, prefix{}, enabled_log_level, &_delegate},
		_delegate{serial::ComPort::COM1}
	{
		serial::SerialPort port(serial::ComPort::COM1);
		port.configure();
		this->delegate(&_delegate);
	}

	StaticSerialLogBuf(StaticSerialLogBuf&& other)
		: StaticDelegatingLogBuf<prefix>{lib::move(other)}
		, _delegate{lib::move(other._delegate)}
	{
		this->delegate(&_delegate);
	}

	~StaticSerialLogBuf() {
		this->delegate(nullptr);
	}
private:
	SerialBuf _delegate;
};


constexpr vga_console::color_t color(lib::LogLevel level) {
	switch (level)
	{
	case lib::LogLevel::CRITICAL: return vga_console::red;
	case lib::LogLevel::ERROR: return vga_console::bright_red;
	case lib::LogLevel::WARNING: return vga_console::yellow;
	case lib::LogLevel::NOTICE: return vga_console::green;
	case lib::LogLevel::INFO: return vga_console::bright_green;
	case lib::LogLevel::DEBUG: return vga_console::bright_green;
	default: panic("Unknown log level");
	}
}

template<class Char, Char ...Prefix>
class StaticVgaDelegatingLogBuf;

template<class Char, Char ...Prefix>
class StaticVgaDelegatingLogBuf<lib::basic_static_string<Char, Prefix...>>: public StaticDelegatingLogBuf<lib::static_string<Prefix...>> {
private:
	using prefix = lib::static_string<Prefix...>;
public:
	StaticVgaDelegatingLogBuf(lib::LogLevel log_level, const prefix&,
			   lib::LogLevel& enabled_log_level,
			   vga_console::color_t color = vga_console::color_t::white)
			   : StaticDelegatingLogBuf<prefix>{log_level, prefix{}, enabled_log_level, &_delegate}
			   , _expected_color{color}
	{
		this->delegate(&_delegate);
	}

	StaticVgaDelegatingLogBuf(StaticVgaDelegatingLogBuf&& other)
		: StaticDelegatingLogBuf<prefix>{lib::move(other)}
		, _expected_color{other._expected_color}
		, _saved_color{other._saved_color}
		, _delegate{lib::move(other._delegate)}
	{
		this->delegate(&_delegate);
	}

	~StaticVgaDelegatingLogBuf() {
		this->delegate(nullptr);
	}

protected:
	void handle_pre_prefix() override {
		_saved_color = vga_console::get_fg();
		set_fg(_expected_color);
	}

	void handle_end_line() override {
		set_fg(_saved_color);
	}


private:
	const vga_console::color_t _expected_color;
	vga_console::color_t _saved_color{_expected_color};
	VgaBuf _delegate{};

};

}
