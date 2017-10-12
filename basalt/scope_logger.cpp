#include "include/basalt/scope_logger.hpp"

basalt::ScopeLogger::ScopeLogger(std::ostream& stream, const char* scope_name)
	: _stream(stream), _scope_name(scope_name)
{
	_stream << "Entering " << _scope_name << std::endl;
}


basalt::ScopeLogger::~ScopeLogger()
{
	_stream << "Leaving " << _scope_name << std::endl;
}

