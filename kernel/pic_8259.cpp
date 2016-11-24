#include <bolgenos-ng/pic_8259.hpp>

#include <bolgenos-ng/string.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/irq.hpp>


namespace {


struct pic_8259: public pic::pic_device {
	pic_8259(): pic_device() {
		pic_device::min_irq_vector = 0x20;
	}


	pic_8259(const pic_8259&) = delete;


	pic_8259& operator =(const pic_8259&) = delete;


	virtual ~pic_8259() {
	}

	virtual void setup();


	virtual void end_of_interrupt(irq::irq_t vector);
};


pic_8259 driver_object;


using command_offset = lib::integral_constant<int, 0>;
using data_offset = lib::integral_constant<int, 1>;


enum port_type: uint16_t {
	master		= 0x20,
	master_comm	= master + command_offset::value,
	master_data	= master + data_offset::value,
	slave		= 0xa0,
	slave_comm	= slave + command_offset::value,
	slave_data	= slave + data_offset::value
};


enum command_type: uint8_t {
	icw_1				= 0x11,
	icw_3_master			= 0x4,
	icw_3_slave			= 0x2,
	icw_4_8086			= 0x1,
	end_of_interrupt		= 0x20
};


} // namespace


pic::pic_device &pic::chip_pic_8259 = driver_object;


void pic_8259::end_of_interrupt(irq::irq_t vector) {
	if (vector > 8 + 0x20) {
		outb(port_type::slave_comm, command_type::end_of_interrupt);
	}

	outb(port_type::master_comm, command_type::end_of_interrupt);
}


void pic_8259::setup() {
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
}

