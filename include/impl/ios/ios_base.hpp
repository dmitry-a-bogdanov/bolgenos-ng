#pragma once

namespace std
{

namespace details
{

// all possible flags. ios_base will provide only supported
enum _fmtflags {
	_F_dec = 1 << 0,
	_F_oct = 1 << 1,
	_F_hex = 1 << 2,
	_F_basefield = _F_dec | _F_oct | _F_hex,


	_F_scientific = 1 << 3,
	_F_fixed = 1 << 4,
	_F_floatfield = _F_scientific | _F_fixed,

	_F_left = 1 << 6,
	_F_right = 1 << 7,
	_F_internal = 1 << 8,
	_F_adjustfield = _F_left | _F_right | _F_internal,

	_F_boolalpha = 1 << 9,
	_F_showbase = 1 << 10,
	_F_showpoint = 1 << 11,
	_F_showpos = 1 << 12,
	_F_skipws = 1 << 13,
	_F_unitbuf = 1 << 14,
	_F_uppercase = 1 << 15
};

} // namespace details


class ios_base
{
public:
	ios_base(const ios_base&) = delete;
	virtual ~ios_base() = default;


	typedef details::_fmtflags fmtflags;

	static constexpr fmtflags dec = fmtflags::_F_dec;
	static constexpr fmtflags hex = fmtflags::_F_hex;
	static constexpr fmtflags basefield = fmtflags::_F_basefield;
	static constexpr fmtflags boolalpha = fmtflags::_F_boolalpha;


	fmtflags flags() const { return _flags; }

	fmtflags flags(fmtflags flags);

	fmtflags setf(fmtflags flags);
	fmtflags setf(fmtflags flags, fmtflags mask);

	void unsetf(fmtflags flags);

protected:
	ios_base() = default;

private:
	fmtflags _flags{};
};


} // namespace std
