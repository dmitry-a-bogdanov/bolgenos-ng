#include "ext/scoped_format_guard.hpp"

using namespace lib;

ScopedFormatGuard::ScopedFormatGuard(ostream& guarded_stream):
	guarded_stream_(guarded_stream),
	backup_(nullptr)
{
	backup_.copyfmt(guarded_stream_);
}

ScopedFormatGuard::~ScopedFormatGuard() {
	guarded_stream_.copyfmt(backup_);
}
