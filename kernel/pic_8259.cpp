#include <bolgenos-ng/pic_8259.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/string.h>

#include <bolgenos-ng/irq.hpp>

static void pic_setup();
static void pic_end_of_interrupt(irq_t irq);

struct pic_device pic_8259 = {
	.setup				= pic_setup,
	.end_of_interrupt		= pic_end_of_interrupt,
	.min_irq_vector			= 0x20
};


#define PIC_COMM_OFFSET			(0)
#define PIC_DATA_OFFSET			(1)

typedef enum {
	pic_master		= 0x20,
	pic_master_comm	= pic_master + PIC_COMM_OFFSET,
	pic_master_data	= pic_master + PIC_DATA_OFFSET,
	pic_slave		= 0xa0,
	pic_slave_comm	= pic_slave + PIC_COMM_OFFSET,
	pic_slave_data	= pic_slave + PIC_DATA_OFFSET
} pic_port_t;

void disable_pic_8259() {
	asm(
		"mov $0xff, %al\n"
		"outb %al, $0xa1\n"
		"outb %al, $0x21\n"
	);
}

typedef enum {
	pic_icw_1			= 0x11,
	pic_icw_3_master		= 0x4,
	pic_icw_3_slave			= 0x2,
	pic_icw_4_8086			= 0x1,
	pic_comm_end_of_interrupt	= 0x20
} pic_comm_t;

void pic_end_of_interrupt(uint8_t irq) {
	if (irq > 8 + 0x20) {
		outb(pic_slave_comm, pic_comm_end_of_interrupt);
	}

	outb(pic_master_comm, pic_comm_end_of_interrupt);
}


void pic_setup() {
	int offset1 = 0x20;
	int offset2 = 0x28;

	outb(pic_master_comm, pic_icw_1);
	outb(pic_slave_comm, pic_icw_1);

	outb(pic_master_data, offset1);
	outb(pic_slave_data, offset2);

	outb(pic_master_data, pic_icw_3_master);
	outb(pic_slave_data, pic_icw_3_slave);

	outb(pic_master_data, pic_icw_4_8086);
	outb(pic_slave_data, pic_icw_4_8086);

	outb(pic_master_data, 0x00);
}

