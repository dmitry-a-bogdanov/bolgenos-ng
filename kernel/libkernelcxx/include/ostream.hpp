#pragma once

#include <cstddef.hpp>


namespace lib {


// forward declaration
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
	/// for caller ostream.
	using manipulator_type = ostream& (ostream&);


	/// Type describing possible format flags.
	enum fmtflags {
		/// Output numbers with base 10.
		dec = 1 << 0,
		/// Output numbers with base 16.
		hex = 1 << 1,
		/// Mask that includes all flags which control base of numbers.
		basefield = dec | hex,
	};


	ostream() = default;


	/// \brief Constructor.
	///
	/// Construct ostream over provided streambuf.
	///
	/// \param sb streambuf that should be base for ostream.
	explicit ostream(streambuf *sb);


	/// Copying of ostream is denied.
	ostream(const ostream&) = delete;


	/// Copying of ostream is denied.
	ostream& operator =(const ostream&) = delete;


	/// \brief Destructor.
	virtual ~ostream();


	/// Insert one character to stream,
	ostream& put(char c);


	/// Copy array of characters to stream.
	ostream& write(const char *str, size_t len);


	/// Print specified character.
	ostream& operator<<(char val);


	/// Print specified pointer.
	ostream& operator<<(void *ptr);


	/// Print specified pointer.
	ostream& operator<<(const void *ptr);


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

	/// Apply formatter to ostream.
	ostream& operator<<(manipulator_type formatter);


	/// \brief Set specific format flags.
	///
	/// The function sets the stream's format flags whose bits are set in
	/// specified argument, leaving unchanged the rest.
	///
	/// \param fl Format flags to be set.
	/// \return The format flags selected in the stream before the call.
	fmtflags setf(fmtflags fl);


	/// \brief Set specific format flags.
	///
	/// The function sets the stream's format flags whose bits are set in
	/// both \p fl and \p mask, and clears the format flags whose bits
	/// are set in \p mask but not in \p fl.
	///
	/// \param fl Format flags to be set.
	/// \param mask Mask containing the flags to be modified.
	/// \return The format flags selected in the stream before the call.
	fmtflags setf(fmtflags fl, fmtflags mask);


	/// \brief Get format flags
	///
	/// The functions returns the format flags currently selected in
	/// the stream.
	///
	/// \return The format flags selected in the stream before the call.
	fmtflags flags() const;

	/// \brief Get field width.
	///
	/// The function returns the current value of the field width.
	///
	/// \return The value of the field width before the call.
	size_t width() const;

	/// \brief Set field width.
	///
	/// The functions sets a new field width for the stream.
	///
	/// \param wide New value for the stream's field width.
	/// \return The value of the field width before the call.
	size_t width(size_t wide);

	/// \brief Get fill character.
	///
	/// The function returns the fill character.
	///
	/// \return The value of the fill character before the call.
	char fill() const;


	/// \brief Set fill character.
	///
	/// The function sets \p fillch as the new fill character.
	/// The fill character is the character used by output insertion
	/// functions to fill spaces when padding results to the field width.
	/// The parametric manipulator \ref setfill can also be used to set
	/// the fill character.
	///
	/// \return The value of the fill character before the call.
	char fill(char fillch);


	/// \brief Copy formatting information.
	///
	/// Copies the values of all the internal members of \p other related
	/// to formatting to the corresponding members of *this.
	///
	/// \return *this
	ostream& copyfmt(ostream& other);


	lib::streambuf* rdbuf() const;
	lib::streambuf* rdbuf(lib::streambuf* sb);

private:
	/// Pointer to underlying streambuf.
	streambuf *streambuf_{nullptr};


	/// Format flags of the stream.
	fmtflags format_ = fmtflags::dec;


	/// Current field's width.
	size_t width_ = 0;


	/// Current fill character.
	char fillch_ = ' ';
};


/// \brief End of line.
///
/// Manipulator puts end of line symbol to output stream.
ostream& endl(ostream &stream);


/// \brief Decimal manipulator.
///
/// Manipulator sets decimal flags in output stream.
ostream& dec(ostream &stream);


/// \brief Hexadecimal manipulator.
///
/// Manipulator sets hexadecimal flags in output stream.
ostream& hex(ostream &stream);


/// Implementation details.
namespace _impl {


/// Object for calling \ref ostream::width(size_t).
struct _setw_type {
	/// Argument for calling \ref ostream::width(size_t).
	size_t width;
};


/// Object for calling \ref ostream::fill(char).
struct _setfill_type {
	/// Argument for calling \ref ostream::fill(char).
	char fillch;
};


/// output operator that does call of \ref ostream::width(size_t).
ostream& operator <<(ostream& stream, const _setw_type& setter);


/// output operator that does call of \ref ostream::fill(char).
ostream& operator <<(ostream& stream, const _setfill_type& setter);


} // namespace _impl


/// \brief Set field width.
///
/// Sets the field width to be used on output operations.
///
/// Behaves as if member \ref ostream::width(size_t) were called with \p n
/// as argument on the stream on which it is inserted as a manipulator.
///
/// \param n Number of characters to be used as field width.
/// \return object of \ref _impl::_setw_type.
_impl::_setw_type setw(size_t n);


/// \brief Set fill character.
///
/// Sets c as the stream's fill character.
///
/// Behaves as if member \ref ostream::fill(char) were called with \p fillch
/// as argument on the stream on which it is inserted as a manipulator.
///
/// \param fillch The new fill character for the stream.
/// \return object of \ref _impl::_setfill_type.
_impl::_setfill_type setfill(char fillch);

} // namespace lib
