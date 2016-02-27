#include <bolgenos-ng/cout.hpp>

#include <bolgenos-ng/printk.h>

namespace {

void print_level(cio::OutStream &stream);


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

cio::OutStream cio::cout;
cio::LogStream cio::ccrit(static_cast<int>(log_level_t::critical));
cio::LogStream cio::cerr(static_cast<int>(log_level_t::error));
cio::LogStream cio::cwarn(static_cast<int>(log_level_t::warning));
cio::LogStream cio::cinfo(static_cast<int>(log_level_t::info));
cio::LogStream cio::cnotice(static_cast<int>(log_level_t::notice));


cio::OutStream::OutStream() {
}


cio::OutStream::~OutStream() {
}


void cio::OutStream::set_newline_callback(newline_callback_type cb) {
	newline_callback_ = cb;
}

cio::OutStream& cio::OutStream::operator <<(const char *string) {
	exec_newline_callback_if_needed();
	printk("%s", string);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(void *ptr) {
	exec_newline_callback_if_needed();
	printk("%lu", (long unsigned) ptr);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(signed long val) {
	exec_newline_callback_if_needed();
	printk("%li", val);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(signed int val) {
	exec_newline_callback_if_needed();
	return *this << (signed long) val;
}


cio::OutStream& cio::OutStream::operator <<(unsigned long val) {
	exec_newline_callback_if_needed();
	printk("%lu", val);
	return *this;
}


cio::OutStream& cio::OutStream::operator <<(unsigned int val) {
	exec_newline_callback_if_needed();
	return *this << (unsigned long) val;
}


cio::OutStream& cio::OutStream::operator <<(format_func_type func) {
	return func(*this);
}

void cio::OutStream::exec_newline_callback_if_needed() {
	if (run_newline_callback_) {
		// reset flag first in order to avoid recursion!
		run_newline_callback_ = false;
		if (newline_callback_)
			newline_callback_(*this);
	}
}

cio::OutStream& cio::endl(OutStream &stream) {
	stream << "\n";
	stream.run_newline_callback_ = true;
	return stream;
}


cio::LogStream::LogStream(int level)
		: OutStream() {
	log_level(level);
	set_newline_callback(print_level);
}


cio::LogStream::~LogStream() {
}


void cio::LogStream::log_level(int level) {
	log_level_ = level;
}


int cio::LogStream::log_level() const {
	return log_level_;
}



int cio::LogStream::system_log_level_ = static_cast<int>(log_level_t::notice);


namespace {

void print_level(cio::OutStream &stream) {
	cio::LogStream *log_stream = reinterpret_cast<cio::LogStream *>(&stream);
	const char *level_name = level_to_string(
			static_cast<log_level_t>(log_stream->log_level())
		);
	stream << "[" << level_name << "] ";
}


} // namespace
