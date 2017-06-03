#include "pic_8259.hpp"

#include <bolgenos-ng/string.h>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/error.h>


namespace {


constexpr int COMMAND_OFFSET = 0;
constexpr int DATA_OFFSET = 1;


enum port_type: uint16_t {
	master		= 0x20,
	master_comm	= master + COMMAND_OFFSET,
	master_data	= master + DATA_OFFSET,
	slave		= 0xa0,
	slave_comm	= slave + COMMAND_OFFSET,
	slave_data	= slave + DATA_OFFSET
};


enum command_type: uint8_t {
	icw_1				= 0x11,
	icw_3_master			= 0x4,
	icw_3_slave			= 0x2,
	icw_4_8086			= 0x1,
	end_of_interrupt		= 0x20
};


} // namespace


int devices::PIC8259::min_irq_vector() const
{
	return 0x20;
}


void devices::PIC8259::end_of_interrupt(irq::irq_t vector)
{
	if (vector > 8 + 0x20) {
		outb(port_type::slave_comm, command_type::end_of_interrupt);
	}

	outb(port_type::master_comm, command_type::end_of_interrupt);
}


void devices::PIC8259::initialize_controller()
{
	if (_controller_initialized) {
		panic("Attempt to initialize PIC8259 twice!");
	}
	int offset1 = 0x20;
	int offset2 = 0x28;

	outb(port_type::master_comm, command_type::icw_1);
	outb(port_type::slave_comm, command_type::icw_1);

	outb(port_type::master_data, offset1);
	outb(port_type::slave_data, offset2);

	outb(port_type::master_data, command_type::icw_3_master);
	outb(port_type::slave_data, command_type::icw_3_slave);

	outb(port_type::master_data, command_type::icw_4_8086);
	outb(port_type::slave_data, command_type::icw_4_8086);

	outb(port_type::master_data, 0x00);

	_controller_initialized = true;
}

