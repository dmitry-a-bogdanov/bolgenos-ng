#include <lib/ostream.hpp>

#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/vga_console.hpp>

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


void lib::ostream::set_newline_callback(newline_callback_type cb) {
	newline_callback_ = cb;
}


lib::ostream& lib::ostream::operator <<(char val) {
	exec_newline_callback_if_needed();
	vga_console::putc(val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(const char *string) {
	exec_newline_callback_if_needed();
	printk("%s", string);
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


lib::ostream& lib::ostream::operator <<(unsigned char val) {
	exec_newline_callback_if_needed();
	return *this << static_cast<unsigned short>(val);
}


lib::ostream& lib::ostream::operator <<(short val) {
	exec_newline_callback_if_needed();
	return *this << static_cast<int>(val);
}


lib::ostream& lib::ostream::operator <<(unsigned short val) {
	exec_newline_callback_if_needed();
	return *this << static_cast<unsigned int>(val);
}


lib::ostream& lib::ostream::operator <<(int val) {
	exec_newline_callback_if_needed();
	return *this << static_cast<long>(val);
}


lib::ostream& lib::ostream::operator <<(unsigned int val) {
	exec_newline_callback_if_needed();
	return *this << static_cast<unsigned long>(val);
}


lib::ostream& lib::ostream::operator <<(long val) {
	exec_newline_callback_if_needed();
	printk("%li", val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(unsigned long val) {
	exec_newline_callback_if_needed();
	printk("%lu", val);
	return *this;
}


lib::ostream& lib::ostream::operator <<(void *ptr) {
	exec_newline_callback_if_needed();
	printk("%lu", reinterpret_cast<long unsigned>(ptr));
	return *this;
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
