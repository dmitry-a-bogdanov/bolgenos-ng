#include <string.hpp>

#include <memory.hpp>

using namespace lib;

string lib::operator+(const lib::string& lhs, const lib::string& rhs)
{
	const size_t len = lhs.size() + rhs.size();
	unique_ptr<char[]> tmp = make_unique<char[]>(len + 1);
	char* ptr = tmp.get();
	strncpy(ptr, lhs.c_str(), lhs.size());
	ptr += lhs.size();
	strncpy(ptr, rhs.c_str(), rhs.size());
	ptr += rhs.size();
	*(++ptr) = '\0';
	return lib::string{tmp.get()};
}
