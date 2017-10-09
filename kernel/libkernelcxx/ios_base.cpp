#include <impl/ios/ios_base.hpp>

#include <utility>

namespace std
{

ios_base::~ios_base() = default;

ios_base::fmtflags ios_base::flags() const
{
	return _flags;
}

ios_base::fmtflags ios_base::flags(fmtflags flags)
{
	return std::exchange(_flags, flags);
}

ios_base::fmtflags ios_base::setf(fmtflags flags)
{
	return std::exchange(_flags, static_cast<fmtflags>(_flags | flags));
}

ios_base::fmtflags ios_base::setf(fmtflags flags, fmtflags mask)
{
	return std::exchange(_flags, static_cast<fmtflags>((_flags & ~mask)|(flags & mask)));
}

void ios_base::unsetf(fmtflags flags)
{
	setf(static_cast<fmtflags>(0), flags);
}


ios_base& dec(ios_base& ios)
{
	ios.setf(ios_base::dec, ios_base::basefield);
	return ios;
}

ios_base& hex(ios_base& ios)
{
	ios.setf(ios_base::hex, ios_base::basefield);
	return ios;
}

ios_base& oct(ios_base& ios)
{
	ios.setf(ios_base::oct, ios_base::basefield);
	return ios;
}

ios_base& boolalpha(ios_base& ios)
{
	ios.setf(ios_base::boolalpha);
	return ios;
}

}
