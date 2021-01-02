#pragma once

#include <cstdint.hpp>

// forward declaration
namespace lib { class ostream; }


namespace ps2 {

/// \brief status of PS/2 IO operation.
///
/// This enum holds status of PS/2 IO operations that are not guarantied to be
///	successfully performed.
enum class ErrorCode {
	ok		, /*!< No erros detected. */
	wrong_ack	, /*!< Wrong ack is received. */
	timeout		, /*!< Operation was not finished in time. */
	unknown		, /*!< Unknown error. */
};

lib::ostream& operator<<(lib::ostream& out, const ErrorCode& code);


class Conf {
public:

	// PS/2 configuration byte bits
	enum bits: uint8_t {
		int_first	= 1 << 0,
		int_second	= 1 << 1,
		system		= 1 << 2,
		clock_first	= 1 << 4,
		clock_second	= 1 << 5,
		translation	= 1 << 6
	};

	inline
	explicit Conf(uint8_t value)
		: conf_byte_(value)
	{

	}

	inline
	bool is_set(bits field) const
	{
		return conf_byte_ & field;
	}

	inline
	void set(bits field)
	{
		conf_byte_ |= field;
	}

	inline
	void unset(bits field)
	{
		conf_byte_ &= (~field);
	}


	inline
	void reset(uint8_t value)
	{
		conf_byte_ = value;
	}

	inline
	uint8_t get() const
	{
		return conf_byte_;
	}

	virtual ~Conf() {}
private:
	uint8_t conf_byte_ = 0;
};


} // namespace ps2
