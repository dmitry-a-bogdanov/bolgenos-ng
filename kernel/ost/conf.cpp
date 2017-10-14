#include <bolgenos-ng/ost/conf.hpp>
#include <bolgenos-ng/log.hpp>


namespace ost::Conf
{

static std::ostream* _good_stream = &lib::cinfo;
static std::ostream* _bad_stream = &lib::ccrit;
static std::ostream* _debug_stream = &lib::cinfo;

std::ostream& bad_stream()
{
	return *_bad_stream;
}

std::ostream& good_stream()
{
	return *_good_stream;
}

std::ostream& debug_stream()
{
	return *_debug_stream;
}

void bad_stream(std::ostream& out)
{
	_bad_stream = &out;
}

void good_stream(std::ostream& out)
{
	_good_stream = &out;
}

void debug_stream(std::ostream& out)
{
	_debug_stream = &out;
}

}
