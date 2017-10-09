#include <ostream>

#include <type_traits>
#include <utility>

#include <bolgenos-ng/string.h>

#include "../lib/streambuf.hpp"
#include "../lib/vga_buf.hpp"
#include "../lib/vga_log_buf.hpp"


namespace {


template<typename T>
void show_numerical_value(std::ostream& stream, T value);


void fill_field(std::ostream& stream, size_t width, size_t already_have);


} // namespace


std::ostream::ostream(streambuf *sb)
	: streambuf_(sb) {
}


std::ostream::~ostream() {
}


std::ostream& std::ostream::put(char c) {
	streambuf_->sputc(c);
	return *this;
}


std::ostream& std::ostream::write(const char *str, size_t len) {
	streambuf_->sputn(str, len);
	return *this;
}


std::ostream& std::ostream::operator <<(char val) {
	fill_field(*this, width_, 1);
	put(val);
	return *this;
}


std::ostream& std::ostream::operator <<(const char *string) {
	size_t len = strlen(string);
	fill_field(*this, width_, len);
	while (*string) {
		put(*string);
		++string;
	}
	return *this;
}


std::ostream& std::ostream::operator <<(bool value) {
	if (value)
		*this << "true";
	else
		*this << "false";
	return *this;
}


std::ostream& std::ostream::operator <<(unsigned char val) {
	show_numerical_value(*this, val);
	return *this;
}


std::ostream& std::ostream::operator <<(short val) {
	show_numerical_value(*this, val);
	return *this;
}


std::ostream& std::ostream::operator <<(unsigned short val) {
	show_numerical_value(*this, val);
	return *this;
}


std::ostream& std::ostream::operator <<(int val) {
	show_numerical_value(*this, val);
	return *this;
}


std::ostream& std::ostream::operator <<(unsigned int val) {
	show_numerical_value(*this, val);
	return *this;
}


std::ostream& std::ostream::operator <<(long val) {
	show_numerical_value(*this, val);
	return *this;
}


std::ostream& std::ostream::operator <<(unsigned long val) {
	show_numerical_value(*this, val);
	return *this;
}


std::ostream& std::ostream::operator <<(void *ptr) {
	auto integer_value = reinterpret_cast<size_t>(ptr);
	return *this << integer_value;
}


std::ostream& std::ostream::operator <<(manipulator_type func) {
	return func(*this);
}


std::ostream::fmtflags std::ostream::setf(fmtflags fl) {
	fmtflags old_flags = format_;

	format_ = static_cast<fmtflags>(format_ | fl);

	return old_flags;
}


std::ostream::fmtflags std::ostream::setf(fmtflags fl, fmtflags mask) {
	fmtflags former_format = format_;

	format_ = static_cast<fmtflags>(format_ & ~mask);
	format_ = static_cast<fmtflags>(format_ | fl);

	return former_format;
}


std::ostream::fmtflags std::ostream::flags() const {
	return format_;
}


size_t std::ostream::width() const {
	return width_;
}


size_t std::ostream::width(size_t wide) {
	std::swap(wide, width_);
	return wide;
}


char std::ostream::fill() const {
	return fillch_;
}


char std::ostream::fill(char fillch) {
	std::swap(fillch, fillch_);
	return fillch;
}


std::ostream& std::ostream::copyfmt(std::ostream& other) {
	fillch_ = other.fillch_;
	format_ = other.format_;
	width_ = other.width_;
	return *this;
}


std::ostream& std::dec(ostream &stream) {
	stream.setf(ostream::dec, ostream::basefield);
	return stream;
}


std::ostream& std::hex(ostream &stream) {
	stream.setf(ostream::hex, ostream::basefield);
	return stream;
}


std::ostream& std::endl(ostream &stream) {
	stream.put('\n');
	return stream;
}


std::ostream&
std::_impl::operator <<(ostream& stream, const _setw_type& setter) {
	stream.width(setter.width);
	return stream;
}


std::ostream&
std::_impl::operator <<(ostream& stream, const _setfill_type& setter) {
	stream.fill(setter.fillch);
	return stream;
}


std::_impl::_setw_type std::setw(size_t n) {
	return { n };
}


std::_impl::_setfill_type std::setfill(char fillch) {
	return { fillch };
}


void lib::set_log_level(log_level_type log_level) {
	lib::_impl::vga_log_buf::set_system_log_level(log_level);
}


lib::log_level_type lib::get_log_level() {
	return lib::_impl::vga_log_buf::get_system_log_level();
}




namespace {


lib::_impl::vga_buf plain_vga_buf;
lib::_impl::vga_log_buf crit_buf(lib::log_level_type::critical, "[C] ");
lib::_impl::vga_log_buf err_buf(lib::log_level_type::error, "[E] ");
lib::_impl::vga_log_buf warn_buf(lib::log_level_type::warning, "[W] ");
lib::_impl::vga_log_buf notice_buf(lib::log_level_type::notice, "[N] ");
lib::_impl::vga_log_buf info_buf(lib::log_level_type::info, "[I] ");
lib::_impl::vga_log_buf debug_buf(lib::log_level_type::debug, "[D] ");


} // namespace




std::ostream std::cout(&plain_vga_buf);
std::ostream lib::ccrit(&crit_buf);
std::ostream lib::cerr(&err_buf);
std::ostream lib::cwarn(&warn_buf);
std::ostream lib::cnotice(&notice_buf);
std::ostream lib::cinfo(&info_buf);
std::ostream lib::cdebug(&debug_buf);


namespace {


void fill_field(std::ostream& stream, size_t width, size_t already_have) {
	char fillch = stream.fill();
	while (width-- > already_have) {
		stream.put(fillch);
	}
}


constexpr size_t MAX_DEC_LENGTH = 20 + 1 /* sign */;


char to_printable(unsigned char digit) {
	if (digit < 10) {
		return '0' + digit;
	} else {
		return 'a' + digit - 10;
	}
}


template<typename T>
void show_numerical_value(std::ostream& stream, T value) {
	using value_type = T;
	using unsigned_value_type = typename std::make_unsigned<value_type>::type;

	unsigned_value_type unsigned_value = 0;
	unsigned_value_type base = 10;
	auto basefield = stream.flags() & std::ostream::basefield;
	if (basefield == std::ostream::hex) {
		unsigned_value = static_cast<unsigned_value_type>(value);
		base = 16;
	} else {
		if (value >= 0) {
			unsigned_value = static_cast<unsigned_value_type>(value);
		} else {
			unsigned_value = static_cast<unsigned_value_type>(0) - value;
		}
	}

	char buffer[MAX_DEC_LENGTH];
	size_t digits = 0;

	while (unsigned_value) {
		unsigned char digit = unsigned_value % base;
		buffer[digits++] = to_printable(digit);
		unsigned_value /= base;
	}

	if (digits == 0) {
		buffer[digits++] = '0';
	}

	if (basefield == std::ostream::dec && value < 0) {
		buffer[digits++] = '-';
	}

	fill_field(stream, stream.width(), digits);

	do {
		stream.put(buffer[--digits]);
	} while(digits);
}


} // namespace
