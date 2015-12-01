#ifndef __BOLGENOS_NG__PS_2_H__
#define __BOLGENOS_NG__PS_2_H__ (1)

enum ps2_dev_idx {
	ps2_dev_1		= 0x0,
	ps2_dev_2		= 0x1
};

enum ps2_port {
	ps2_data_port		= 0x60,
	ps2_status_reg		= 0x64,
	ps2_command_reg		= 0x64
};

// PS/2 status register bits
enum ps2_sr {
	ps2_sr_out_buf_status	= 1 << 0,
	ps2_sr_in_buf_status	= 1 << 1,
	ps2_sr_post_passed	= 1 << 2,
	ps2_sr_comm_or_data	= 1 << 3,
	// unknown		= 1 << 4,
	// unknown		= 1 << 5,
	ps2_sr_timeout		= 1 << 6,
	ps2_sr_parity_error	= 1 << 7
};

// PS/2 configuration byte bits
enum ps2_cb {
	ps2_cb_int_first	= 1 << 0,
	ps2_cb_int_second	= 1 << 1,
	ps2_cb_system		= 1 << 2,
	ps2_cb_clock_first	= 1 << 4,
	ps2_cb_clock_second	= 1 << 5,
	ps2_cb_translation	= 1 << 6
};

enum ps2_command {
	ps2_cmd_read_cb		= 0x20,
	ps2_cmd_write_cb	= 0x60,

	ps2_cmd_self_test	= 0xaa,
	ps2_cmd_port_test_1	= 0xab,
	ps2_cmd_port_test_2	= 0xa9,

	ps2_cmd_disable_1	= 0xad,
	ps2_cmd_enable_1	= 0xae,
	ps2_cmd_disable_2	= 0xa7,
	ps2_cmd_enable_2	= 0xa8
};

enum ps2_reply {
	ps2_rpl_self_test_ok	= 0x55,
	ps2_rpl_self_test_fail	= 0xfc,
	ps2_rpl_port_test_ok	= 0x0
};

void ps2_enable_dev(enum ps2_dev_idx idx);
void ps2_disable_dev(enum ps2_dev_idx idx);

void ps2_init();

void handle_first_ps2_dev_int();
void handle_second_ps2_dev_int();

#endif // __BOLGENOS_NG__PS_2_H__
