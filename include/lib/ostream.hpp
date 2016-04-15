#pragma once


#include <bolgenos-ng/stdtypes.hpp>


namespace lib {


class streambuf;


/// \brief Output stream class
///
/// Class that provides base interface to output.
class ostream {
public:
	/// \brief Type of formatting function pointer.
	///
	/// Formatting functions are used for implementing special behavior
	/// like `cout << endl;` or `cout << hex`. When `operator <<` is
	/// called for object of this type, passed function will be invoked
	/// for caller OutStream.
	using manipulator_type = ostream& (ostream&);


	enum fmtflags {
		dec = 1 << 0,
		hex = 1 << 1,
		basefield = dec | hex,
	};


	/// \brief Default constructor.
	ostream() = delete;


	explicit ostream(streambuf *sb);


	/// Copying of OutStream is denied.
	ostream(const ostream&) = delete;


	/// Copying of OutStream is denied.
	ostream& operator =(const ostream&) = delete;


	/// \brief Descructor.
	virtual ~ostream();


	/// Insert one character to stream,
	ostream& put(char c);


	/// Copy array of characters to stream.
	ostream& write(const char *str, size_t len);


	/// Print specified character.
	ostream& operator<<(char val);


	/// Print specified string.
	ostream& operator<<(const char *string);


	/// Print specified boolean value.
	ostream& operator<<(bool val);


	/// Print specified unsigned char.
	ostream& operator<<(unsigned char val);


	/// Print specified short.
	ostream& operator<<(short val);


	/// Print specified unsigned short.
	ostream& operator<<(unsigned short val);


	/// Print specified int.
	ostream& operator<<(int val);


	/// Print specified unsigned int.
	ostream& operator<<(unsigned int val);


	/// Print specified long.
	ostream& operator<<(long val);


	/// Print specified unsigned long.
	ostream& operator<<(unsigned long val);


	/// Print specified pointer.
	ostream& operator<<(void *ptr);


	/// Apply formatter to OutStream.
	ostream& operator<<(manipulator_type formatter);


	fmtflags setf(fmtflags fl);


	fmtflags setf(fmtflags fl, fmtflags mask);


	fmtflags flags() const;


	size_t width() const;


	size_t width(size_t wide);


	char fill() const;


	char fill(char fillch);
private:
	streambuf *streambuf_;


	fmtflags format_ = fmtflags::dec;


	size_t width_ = 0;


	char fillch_ = ' ';
};


/// \brief End of line.
///
/// The function prints end of line symbol and sets flag for newline callback in
/// OutStream.
ostream& endl(ostream &stream);


ostream& dec(ostream &stream);


ostream& hex(ostream &stream);


namespace _impl {


struct _setw_type {
	size_t width;
};


struct _setfill_type {
	char fillch;
};


ostream& operator <<(ostream& stream, const _setw_type& setter);


ostream& operator <<(ostream& stream, const _setfill_type& setter);


} // namespace _impl


_impl::_setw_type setw(size_t n);


_impl::_setfill_type setfill(char fillch);


enum log_level_type: int {
	critical	= 0,
	error		= 1,
	warning		= 2,
	notice		= 3,
	info		= 4,
};


log_level_type get_log_level();


void set_log_level(log_level_type log_level);


/// \brief Console output object.
///
/// Object of OutStream is to be used for printing to VGA console.
extern ostream cout;


/// Output object for critical error messages.
extern ostream ccrit;


/// Output object for error messages.
extern ostream cerr;


/// Output object for warning messages.
extern ostream cwarn;


/// Output object for notice messages.
extern ostream cnotice;


/// Output object for info messages.
extern ostream cinfo;



} // namespace cio
