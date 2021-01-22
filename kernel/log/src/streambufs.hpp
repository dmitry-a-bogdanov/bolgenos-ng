#pragma once

namespace lib {
class streambuf;
}

namespace log {

extern lib::streambuf& sbuf_vga_console;
extern lib::streambuf& sbuf_serial_console;

}