#include <ext/snprintf_stream.hpp>

lib::SnprintfStream::SnprintfStream(char* buf, size_t max_len)
	: ostream{&_buf},
	_buf{buf, max_len}
{
}

lib::SnprintfStream::Buf::Buf(char* buf, size_t max_len):
	_buf{buf},
	_last{buf + max_len - 1}
{
	if (max_len == 0) {
		_last = _buf;
	}
	*_buf = '\0';
}

int lib::SnprintfStream::Buf::overflow(const int c)
{
	if (_buf == _last) {
		return c;
	}
	*(_buf++) = c;
	*_buf = '\0';
	return c;
}

size_t lib::SnprintfStream::Buf::xsputn(const char* s, const size_t n)
{
	if (_buf == _last) {
		return n;
	}
	size_t written = 0;
	while (_buf != _last && written != n) {
		*(_buf++) = *(s++);
		++written;
	}
	*_buf = '\0';
	return n;
}
