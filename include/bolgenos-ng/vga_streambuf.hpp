#pragma once
#if 0
#include <streambuf>
#include "io/vga/types.hpp"

template<>
struct std::char_traits<bolgenos::io::vga::char_t>
{
	using char_type = bolgenos::io::VgaChar;
	using int_type = int;
	using pos_type = std::size_t;
	using off_type = std::ptrdiff_t;
};

namespace bolgenos
{
namespace io
{

class VgaConsoleStreambuf: public std::basic_streambuf<VgaChar>
{
};

}
}
#endif
