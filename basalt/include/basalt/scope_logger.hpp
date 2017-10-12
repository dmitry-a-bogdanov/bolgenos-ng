#pragma once

#include <ostream>

namespace basalt
{

class ScopeLogger
{
public:
	ScopeLogger() = delete;
	ScopeLogger(const ScopeLogger&) = delete;
	ScopeLogger& operator=(const ScopeLogger&) = delete;

	ScopeLogger(std::ostream& stream, const char* scope_name);
	virtual ~ScopeLogger();
protected:
	std::ostream& _stream;
	const char *_scope_name;
};

}
