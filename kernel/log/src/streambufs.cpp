#include "streambufs.hpp"

#include <streambuf.hpp>
#include "vga_buf.hpp"
#include "serial_buf.hpp"

namespace {

VgaBuf sbuf_vga_console_impl{};
SerialBuf sbuf_serial_console_impl{[](){
	serial::SerialPort com_port(serial::ComPort::COM1);
	com_port.configure();
	return com_port;
}()};

}

lib::streambuf& log::sbuf_vga_console{sbuf_vga_console_impl};
lib::streambuf& log::sbuf_serial_console{sbuf_serial_console_impl};
