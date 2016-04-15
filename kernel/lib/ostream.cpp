#include <lib/ostream.hpp>

// #include <bolgenos-ng/printk.h>

#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.hpp>

#include <lib/type_traits.hpp>
#include <lib/utility.hpp>

#include "streambuf.hpp"
#include "vga_buf.hpp"
#include "vga_log_buf.hpp"


namespace {


template<typename T>
void show_numerical_value(lib::ostream& stream, T value);


void fill_field(lib::ostream& stream, size_t width, size_t already_have);


} // namespace


lib::ostream::ostream(streambuf *sb)
	: streambuf_(sb) {
}


lib::ostream::~ostream() {
}


lib::ostream& lib::ostream::put(char c) {
	streambuf_->sputc(c);
	return *this;
}


lib::ostream& lib::ostream::write(const char *str, size_t len) {
	streambuf_->sputn(str, len);
	return *this;
}


lib::ostream& lib::ostream::operator <<(char val) {
	fill_field(*this, width_, 1);
	put(val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(const char *string) {
	size_t len = strlen(string);
	fill_field(*this, width_, len);
	while (*string) {
		put(*string);
		++string;
	}
	return *this;
}


lib::ostream& lib::ostream::operator <<(bool value) {
	if (value)
		*this << "true";
	else
		*this << "false";
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned char val) {
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(short val) {
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned short val) {
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(int val) {
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned int val) {
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(long val) {
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned long val) {
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(void *ptr) {
	auto integer_value = reinterpret_cast<size_t>(ptr);
	return *this << integer_value;
}


lib::ostream& lib::ostream::operator <<(manipulator_type func) {
	return func(*this);
}


lib::ostream::fmtflags lib::ostream::setf(fmtflags fl) {
	fmtflags old_flags = format_;

	format_ = static_cast<fmtflags>(format_ | fl);

	return old_flags;
}


lib::ostream::fmtflags lib::ostream::setf(fmtflags fl, fmtflags mask) {
	fmtflags former_format = format_;

	format_ = static_cast<fmtflags>(format_ & ~mask);
	format_ = static_cast<fmtflags>(format_ | fl);

	return former_format;
}


lib::ostream::fmtflags lib::ostream::flags() const {
	return format_;
}


size_t lib::ostream::width() const {
	return width_;
}


size_t lib::ostream::width(size_t wide) {
	lib::swap(wide, width_);
	return wide;
}


char lib::ostream::fill() const {
	return fillch_;
}


char lib::ostream::fill(char fillch) {
	lib::swap(fillch, fillch_);
	return fillch;
}


lib::ostream& lib::dec(ostream &stream) {
	stream.setf(ostream::fmtflags::dec, ostream::fmtflags::basefield);
	return stream;
}


lib::ostream& lib::hex(ostream &stream) {
	stream.setf(ostream::fmtflags::hex, ostream::fmtflags::basefield);
	return stream;
}


lib::ostream& lib::endl(ostream &stream) {
	stream.put('\n');
	return stream;
}


lib::ostream&
lib::_impl::operator <<(ostream& stream, const _setw_type& setter) {
	stream.width(setter.width);
	return stream;
}


lib::ostream&
lib::_impl::operator <<(ostream& stream, const _setfill_type& setter) {
	stream.fill(setter.fillch);
	return stream;
}


lib::_impl::_setw_type lib::setw(size_t n) {
	return { n };
}


lib::_impl::_setfill_type lib::setfill(char fillch) {
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
lib::_impl::vga_log_buf crit_buf(lib::log_level_type::critical, "[CRIT] ");
lib::_impl::vga_log_buf err_buf(lib::log_level_type::error, "[ERR] ");
lib::_impl::vga_log_buf warn_buf(lib::log_level_type::warning, "[WARN] ");
lib::_impl::vga_log_buf notice_buf(lib::log_level_type::notice, "[NOTE] ");
lib::_impl::vga_log_buf info_buf(lib::log_level_type::info, "[info] ");


} // namespace




lib::ostream lib::cout(&plain_vga_buf);
lib::ostream lib::ccrit(&crit_buf);
lib::ostream lib::cerr(&err_buf);
lib::ostream lib::cwarn(&warn_buf);
lib::ostream lib::cnotice(&notice_buf);
lib::ostream lib::cinfo(&info_buf);


namespace {


void fill_field(lib::ostream& stream, size_t width, size_t already_have) {
	char fillch = stream.fill();
	while (width-- > already_have) {
		stream.put(fillch);
	}
}


using max_dec_length = lib::integral_constant<size_t, 20 + 1 /* sign */>;


char to_printable(unsigned char digit) {
	if (digit < 10) {
		return '0' + digit;
	} else {
		return 'a' + digit - 10;
	}
}


template<typename T>
void show_numerical_value(lib::ostream& stream, T value) {
	using value_type = T;
	using unsigned_value_type = typename lib::make_unsigned<value_type>::type;

	unsigned_value_type unsigned_value = 0;
	unsigned_value_type base = 10;
	auto basefield = stream.flags() & lib::ostream::fmtflags::basefield;
	if (basefield == lib::ostream::fmtflags::hex) {
		unsigned_value = static_cast<unsigned_value_type>(value);
		base = 16;
	} else {
		if (value >= 0) {
			unsigned_value = static_cast<unsigned_value_type>(value);
		} else {
			unsigned_value = static_cast<unsigned_value_type>(0) - value;
		}
	}

	char buffer[max_dec_length::value];
	size_t digits = 0;

	while (unsigned_value) {
		unsigned char digit = unsigned_value % base;
		buffer[digits++] = to_printable(digit);
		unsigned_value /= base;
	}

	if (digits == 0) {
		buffer[digits++] = '0';
	}

	if (basefield == lib::ostream::fmtflags::dec && value < 0) {
		buffer[digits++] = '-';
	}

	fill_field(stream, stream.width(), digits);

	do {
		stream.put(buffer[--digits]);
	} while(digits);
}


} // namespace
