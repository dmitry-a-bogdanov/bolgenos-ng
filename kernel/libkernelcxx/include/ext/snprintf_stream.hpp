#pragma once

#include <ostream.hpp>
#include <streambuf.hpp>

namespace lib {

class SnprintfStream: public ostream {

	class Buf: public streambuf {

	public:
		Buf(char* buf, size_t max_len);
		Buf(const Buf&) = delete;
		Buf& operator=(const Buf&) = delete;
		~Buf() = default;

	protected:
		int overflow(int c) override;

		size_t xsputn(const char* s, size_t n) override;

	private:
		char* _buf;
		char* _last;
	};

public:
	SnprintfStream(char* buf, size_t max_len);

private:
	Buf _buf;
};

}