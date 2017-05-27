#include <bolgenos-ng/ps2/common.hpp>

#include <lib/ostream.hpp>

lib::ostream& ps2::operator<<(lib::ostream& out, const ErrorCode& code)
{
	switch (code) {
	case ErrorCode::ok:
		out << "No error";
		break;
	case ErrorCode::wrong_ack:
		out << "Wrong ack";
		break;
	case ErrorCode::timeout:
		out << "Timeout";
		break;
	default:
		out << "Unknown error";
	}
	return out;
}
