#pragma once

#include <streambuf.hpp>
#include <log/simple_stream_buf.hpp>

class VgaBuf: public SimpleStreamBuf<lib::streambuf> {
protected:
	int overflow(int c) override;
};
