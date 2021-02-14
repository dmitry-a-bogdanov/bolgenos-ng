#pragma once

#include <streambuf.hpp>
#include <serial/serial_port.hpp>
#include "simple_stream_buf.hpp"
#include "delegating_log_buf.hpp"

class SerialBuf: public SimpleStreamBuf<lib::streambuf>
{
public:
	explicit SerialBuf(serial::SerialPort port);

	int overflow(int c) override;
private:
	serial::SerialPort _port;
};
