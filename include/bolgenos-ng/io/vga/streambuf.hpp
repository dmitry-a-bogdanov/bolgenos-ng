#include <streambuf>
#include "types.hpp"

namespace bolgenos
{
namespace io
{
namespace vga
{

class vgabuf: public std::basic_streambuf<bolgenos::io::vga::char_t>
{
	using base_type = std::basic_streambuf<bolgenos::io::vga::char_t>;
public:
	using char_type = base_type::char_type;
	using traits_type = base_type::traits_type;
	using int_type = base_type::int_type;
	using pos_type = base_type::pos_type;
	using off_type = base_type::off_type;

	int_type overflow(int_type ch = traits_type::eof()) override
	{
		return 0;
	}

	base_type* setbuf(char_type *buf, std::streamsize size) override
	{
		return nullptr;
	}

	int sync() override
	{
		return 0;
	}


	std::streamsize xsputn(const char_type* s, std::streamsize count) override
	{
		return 0;
	}
};

}
}
}
