#include <ostream>

#include <streambuf>
#include <bolgenos-ng/vga_streambuf.hpp>


namespace {


class cvga_streambuf: public std::basic_streambuf<char>
{
public:
	cvga_streambuf()
		: basic_streambuf<char>()
	{}

	virtual ~cvga_streambuf() {}
};

cvga_streambuf vga_con_buf;

} // namespace


std::ostream std::cout(&vga_con_buf);
