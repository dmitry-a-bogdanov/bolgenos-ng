#include <lib/ostream.hpp>

// #include <bolgenos-ng/printk.h>

#include <bolgenos-ng/vga_console.hpp>

#include <lib/type_traits.hpp>

namespace {

void print_level(lib::ostream &stream);


enum log_level_t: int {
	critical	= 0,
	error		= 1,
	warning		= 2,
	notice		= 3,
	info		= 4,
};

const char *level_to_string(log_level_t level) {
	switch(level) {
	case log_level_t::critical:
		return "CRIT";
	case log_level_t::error:
		return "ERR";
	case log_level_t::warning:
		return "WARNING";
	case log_level_t::notice:
		return "NOTICE";
	case log_level_t::info:
		return "INFO";
	default:
		return "-";
	}
}

} // namespace

lib::ostream lib::cout;
lib::LogStream lib::ccrit(static_cast<int>(log_level_t::critical));
lib::LogStream lib::cerr(static_cast<int>(log_level_t::error));
lib::LogStream lib::cwarn(static_cast<int>(log_level_t::warning));
lib::LogStream lib::cinfo(static_cast<int>(log_level_t::info));
lib::LogStream lib::cnotice(static_cast<int>(log_level_t::notice));


lib::ostream::ostream() {
}


lib::ostream::~ostream() {
}


lib::ostream& lib::ostream::put(char c) {
	vga_console::putc(c);
	return *this;
}


lib::ostream& lib::ostream::write(const char *str, size_t len) {
	for(size_t idx = 0; idx != len; ++idx) {
		vga_console::putc(str[idx]);
	}
	return *this;
}


void lib::ostream::set_newline_callback(newline_callback_type cb) {
	newline_callback_ = cb;
}


lib::ostream& lib::ostream::operator <<(char val) {
	exec_newline_callback_if_needed();
	put(val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(const char *string) {
	exec_newline_callback_if_needed();
	while (*string) {
		put(*string);
		++string;
	}
	return *this;
}


lib::ostream& lib::ostream::operator <<(bool value) {
	exec_newline_callback_if_needed();
	if (value)
		*this << "true";
	else
		*this << "false";
	return *this;
}


namespace {


using max_dec_length = lib::integral_constant<size_t, 20>;


char to_printable(unsigned char digit) {
	if (digit < 10) {
		return '0' + digit;
	} else {
		return 'a' + digit - 10;
	}
}


template<typename T>
typename lib::enable_if<lib::is_unsigned<T>::value>::type
show_numerical_value(lib::ostream& stream, T value) {
	using value_type = T;
	char buffer[max_dec_length::value];
	size_t digits = 0;
	value_type base = 10;
	if (stream.flags() & lib::ostream::fmtflags::hex) {
		base = 16;
	}
	while (value) {
		unsigned char digit = value % base;
		buffer[digits++] = to_printable(digit);
		value = value / base;
	}
	if (digits) {
		do {
			stream.put(buffer[--digits]);
		} while(digits);
	} else {
		stream.put('0');
	}
}


template<typename T>
typename lib::enable_if<lib::is_signed<T>::value>::type
show_numerical_value(lib::ostream& stream, T value) {
	using value_type = T;
	using unsigned_value_type = typename lib::make_unsigned<value_type>::type;
	unsigned_value_type unsigned_value = 0;
	if (stream.flags() & lib::ostream::hex) {
		unsigned_value = *reinterpret_cast<unsigned_value_type *>(&value);
	} else {
		if (value >= 0) {
			unsigned_value = value;
		} else {
			unsigned_value = static_cast<unsigned_value_type>(0) - value;
			stream.put('-');
		}
	}
	show_numerical_value<unsigned_value_type>(stream, unsigned_value);
}


} // namespace


lib::ostream& lib::ostream::operator <<(unsigned char val) {
	exec_newline_callback_if_needed();
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(short val) {
	exec_newline_callback_if_needed();
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned short val) {
	exec_newline_callback_if_needed();
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(int val) {
	exec_newline_callback_if_needed();
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned int val) {
	exec_newline_callback_if_needed();
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(long val) {
	exec_newline_callback_if_needed();
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned long val) {
	exec_newline_callback_if_needed();
	show_numerical_value(*this, val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(void *ptr) {
	exec_newline_callback_if_needed();
	auto integer_value = reinterpret_cast<size_t>(ptr);
	return *this << integer_value;
}


lib::ostream& lib::ostream::operator <<(manipulator_type func) {
	return func(*this);
}

void lib::ostream::exec_newline_callback_if_needed() {
	if (run_newline_callback_) {
		// reset flag first in order to avoid recursion!
		run_newline_callback_ = false;
		if (newline_callback_)
			newline_callback_(*this);
	}
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


lib::ostream& lib::dec(ostream &stream) {
	stream.setf(ostream::fmtflags::dec, ostream::fmtflags::basefield);
	return stream;
}


lib::ostream& lib::hex(ostream &stream) {
	stream.setf(ostream::fmtflags::hex, ostream::fmtflags::basefield);
	return stream;
}


lib::ostream& lib::endl(ostream &stream) {
	stream << "\n";
	stream.run_newline_callback_ = true;
	return stream;
}


lib::LogStream::LogStream(int level)
		: ostream(), log_level_(level) {
	set_newline_callback(print_level);
}


lib::LogStream::~LogStream() {
}


void lib::LogStream::log_level(int level) {
	log_level_ = level;
}


int lib::LogStream::log_level() const {
	return log_level_;
}



int lib::LogStream::system_log_level_ = static_cast<int>(log_level_t::notice);


namespace {

void print_level(lib::ostream &stream) {
	lib::LogStream *log_stream = reinterpret_cast<lib::LogStream *>(&stream);
	const char *level_name = level_to_string(
			static_cast<log_level_t>(log_stream->log_level())
		);
	stream << "[" << level_name << "] ";
}


} // namespace
