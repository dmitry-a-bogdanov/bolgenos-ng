#pragma once

namespace std
{

class ios_base
{
public:
	ios_base(const ios_base&) = delete;
	virtual ~ios_base();

	enum fmtflags
	{
		dec,
		oct,
		hex,
		basefield = dec|oct|hex|0,
		boolalpha,
	};

	fmtflags flags() const;
	fmtflags flags(fmtflags flags);

	fmtflags setf(fmtflags flags);
	fmtflags setf(fmtflags flags, fmtflags mask);

	void unsetf(fmtflags flags);

protected:
	ios_base() = default;

private:
	fmtflags _flags{};
};

ios_base& dec(ios_base&);
ios_base& oct(ios_base&);
ios_base& hex(ios_base&);
ios_base& boolalpha(ios_base&);

} // namespace std
