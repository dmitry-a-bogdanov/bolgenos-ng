#pragma once

#include "ps2/common.hpp"
#include "ps2_controller.hpp"


namespace ps2 {


struct PS2LineConfiguration
{
	cmd_t cmd_enable;
	cmd_t cmd_disable;
	cmd_t cmd_test;
	bool need_redirection;
	cmd_t cmd_redirect;
	Conf::bits enable_interrups_mask;
	int id;
};


class IPS2Line
{
public:
	IPS2Line(PS2Controller *controller, const PS2LineConfiguration& line_conf)
		: controller_(controller), line_conf_(line_conf)
	{
	}

	IPS2Line() = delete;
	IPS2Line(const IPS2Line&) = delete;
	IPS2Line& operator=(const IPS2Line&) = delete;

	~IPS2Line() = default;

	ps2::Conf::bits enable_interrupts_mask() const
	{
		return line_conf_.enable_interrups_mask;
	}


	/// \brief Send byte to PS/2 device instead of controller.
	///
	/// Function sends byte to the PS/2 device instead of PS/2 controller.
	/// \param byte Byte to be written.
	/// \return Status of PS/2 IO operation.
	ErrorCode send(uint8_t byte);


	/// \brief Send byte to PS/2 device and wait for ack.
	///
	/// Function sends byte to PS/2 device instead of controller and waits for
	///	for receiving ack.
	/// \param line PS/2 device index.
	/// \param byte Byte to be written.
	/// \param ack Byte that should be received as ack.
	/// \return Status of PS/2 IO operation.
	ErrorCode send(uint8_t byte, uint8_t expected_ack);


	void disable();

	/// \brief Enable PS/2 device.
	///
	/// The function enables specified PS/2 device.
	void enable();


	/// \brief Test PS/2 line.
	///
	/// The function tests specified PS/2 line.
	///
	/// \return 1 if test is ok; 0 otherwise.
	int test();

	int id() const
	{
		return line_conf_.id;
	}


	PS2Controller* controller()
	{
		return controller_;
	}


	const PS2Controller* controller() const
	{
		return controller_;
	}

private:
	PS2Controller *controller_ = nullptr;
	PS2LineConfiguration line_conf_;
};


} // namespace ps2
