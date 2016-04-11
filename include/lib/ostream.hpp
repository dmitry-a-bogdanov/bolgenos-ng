#pragma once


#include <bolgenos-ng/stdtypes.hpp>


namespace lib {


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


	/// \brief Type of newline callback.
	///
	/// Type of newline callback function. Newline callback is called when
	/// output is called after putting \ref endl to OutStream.
	using newline_callback_type = void (ostream&);


	/// \brief Default constructor.
	ostream();


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


	/// \brief Set newline callback.
	///
	/// The function sets pointer to newline callback.
	/// \param cb Pointer to callback function.
	virtual void set_newline_callback(newline_callback_type cb);


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


	/// Print specified long long.
	ostream& operator<<(long long val);


	/// Print specified unsigned long long.
	ostream& operator<<(unsigned long long val);


	/// Print specified pointer.
	ostream& operator<<(void *ptr);


	/// Apply formatter to OutStream.
	ostream& operator<<(manipulator_type formatter);
protected:
	/// Execute newline callback if needed.
	///
	/// The function executes registered newline callback function
	/// if flag \ref OutStream::run_newline_callback_ is true and sets
	/// this flag to `false`.
	void exec_newline_callback_if_needed();


	/// Pointer to newline callback function.
	newline_callback_type *newline_callback_ = nullptr;


	/// Run newline callback flag.
	bool run_newline_callback_ = true;



	friend ostream& endl(ostream&);
};


/// \brief Extension of OutStream with support of debug levels.
///
/// Class is inherited from OutStream and adds support of debug levels.
/// Instances of this class are to be used for logging objects like
/// \ref cerr, \ref cinfo etc. Object will produce output if
/// logging level of object is less or equal than system logging level.
class LogStream: public ostream {
public:
	/// Constructor of LogStream object.
	/// \param log_level Numeric logging level.
	explicit LogStream(int log_level);

	/// Contructing of LogStream without specifing logging level is denied.
	LogStream() = delete;


	/// Copying of LogStream is denied.
	LogStream(const LogStream&) = delete;


	/// Copying of LogStream is denied.
	LogStream& operator =(const LogStream&) = delete;


	/// Destructor.
	virtual ~LogStream();


	/// Get logging level of the object.
	int log_level() const;


	/// Set logging level of the object.
	void log_level(int);



	/// \brief Output operator.
	///
	/// Print specified value of variable type if logging level allowes
	/// printing.
	/// \param val Value to be printed.
	template<typename T>
	LogStream &operator<<(const T &val) {
		if (log_level_ > system_log_level_) {
			return *this;
		}
		ostream::operator<<(val);
		return *this;
	}


protected:
	/// \brief Logging level.
	///
	/// Numeric value of logging level.
	int log_level_;


	/// \brief System logging level.
	///
	/// Numeric value of system logging level.
	static int system_log_level_;
};


/// \brief End of line.
///
/// The function prints endline symbol and sets flag for newline callback in
/// OutStream.
ostream& endl(ostream &stream);


/// \brief Console output object.
///
/// Object of OutStream is to be used for printing to VGA console.
extern ostream cout;


/// Output object for critical error messages.
extern LogStream ccrit;


/// Output object for error messages.
extern LogStream cerr;


/// Output object for warning messages.
extern LogStream cwarn;


/// Output object for notice messages.
extern LogStream cnotice;


/// Output object for info messages.
extern LogStream cinfo;


} // namespace cio
