#pragma once

#include <serial/serial_port.hpp>
#include <streambuf.hpp>
#include "simple_stream_buf.hpp"
#include "base_log_buf.hpp"

class SerialBuf: public SimpleStreamBuf<lib::streambuf>
{
public:
	SerialBuf(dev::SerialPort port);

	int overflow(int c) override;
private:
	dev::SerialPort _port;
};

using SerialLogBuf = BaseLogBuf<SerialBuf>;
