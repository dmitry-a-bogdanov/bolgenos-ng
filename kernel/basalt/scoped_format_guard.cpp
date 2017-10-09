#include <basalt/format_guard.hpp>

namespace basalt
{

scoped_format_guard::scoped_format_guard(std::ostream& guarded_stream)
	: _guarded_stream(guarded_stream),
	_backup(nullptr)
{
	_backup.copyfmt(_guarded_stream);
}


scoped_format_guard::~scoped_format_guard()
{
	_guarded_stream.copyfmt(_backup);
}


}
