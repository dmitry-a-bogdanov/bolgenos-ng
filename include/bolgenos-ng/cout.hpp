#pragma once


namespace cio {


class OutStream {
public:
	using format_func_type = OutStream& (OutStream&);
	using newline_callback_type = void (OutStream&);

	OutStream();

	virtual ~OutStream();

	virtual void set_newline_callback(newline_callback_type cb);

	OutStream& operator<<(const char *string);
	OutStream& operator<<(void *ptr);

	OutStream& operator<<(signed long val);
	OutStream& operator<<(unsigned long val);
	OutStream& operator<<(signed int val);
	OutStream& operator<<(unsigned int val);

	OutStream& operator<<(format_func_type formatter);

protected:
	void exec_newline_callback_if_needed();
	newline_callback_type *newline_callback_ = nullptr;
	bool run_newline_callback_ = true;

	friend OutStream& endl(OutStream&);
};


class LogStream: public OutStream {
public:
	explicit LogStream(int log_level);
	virtual ~LogStream();
	int log_level() const;
	void log_level(int);

	template<typename T>
	LogStream &operator<<(T val) {
		if (log_level_ > system_log_level_) {
			return *this;
		}
		OutStream::operator<<(val);
		return *this;
	}
protected:
	int log_level_;
	static int system_log_level_;
};


OutStream& endl(OutStream &stream);

extern OutStream cout;

extern LogStream ccrit;
extern LogStream cerr;
extern LogStream cwarn;
extern LogStream cinfo;
extern LogStream cnotice;

} // namespace cio
