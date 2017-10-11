#pragma once

#include <ostream>

#include <bolgenos-ng/log_level.hpp>

#include "vga_buf.hpp"


namespace lib {
namespace _impl {


class vga_log_buf: public vga_buf {
public:

	vga_log_buf() = delete;
	vga_log_buf(const vga_log_buf&) = delete;
	vga_log_buf& operator=(const vga_log_buf&) = delete;
	vga_log_buf(log_level_type log_level, const char *prefix);
	virtual ~vga_log_buf();

	static log_level_type get_system_log_level();
	static void set_system_log_level(log_level_type log_level);
protected:
	virtual int overflow(int c) override;
private:
	log_level_type log_level_;
	const char *prefix_;
	bool show_header_;

	static log_level_type system_log_level;
};

} // namespace _impl
} // namespace lib
