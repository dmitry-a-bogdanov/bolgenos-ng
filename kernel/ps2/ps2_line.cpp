#include <bolgenos-ng/ps2_line.hpp>

#include <bolgenos-ng/log.hpp>

/// Timeout for output operation.
constexpr int OUTPUT_TIMEOUT = 1 /* ms */;


ps2::ErrorCode ps2::IPS2Line::send(uint8_t byte)
{
	if (line_conf_.need_redirection)
		controller()->send_command(line_conf_.cmd_redirect);

	int can_write = controller()->wait_for_output(OUTPUT_TIMEOUT);
	if (!can_write) {
		return ps2::ErrorCode::timeout;
	}
	controller()->send_byte(byte);
	return ps2::ErrorCode::ok;
}


void ps2::IPS2Line::disable()
{
	controller()->send_command(line_conf_.cmd_disable);
}


void ps2::IPS2Line::enable()
{
	controller()->send_command(line_conf_.cmd_enable);
}


int ps2::IPS2Line::test() {
	controller()->send_command(line_conf_.cmd_test);

	uint8_t reply;
	if (!controller()->receive(SELFTEST_TIMEOUT, &reply)) {
		lib::cerr << "no response to self-test" << lib::endl;
		return 0;
	}
	lib::cinfo << "line test result = " << reply << lib::endl;

	return (reply == test_reply::port_test_ok);
}


ps2::ErrorCode ps2::IPS2Line::send(uint8_t byte, uint8_t expected_ack) {
	ErrorCode ret;
	ret = this->send(byte);
	if (ret != ErrorCode::ok)
		return ret;

	uint8_t ack;
	if (controller()->receive(OUTPUT_TIMEOUT, &ack)) {
		if (ack == expected_ack)
			return ErrorCode::ok;

		return ErrorCode::wrong_ack;
	}
	return ErrorCode::timeout;
}
