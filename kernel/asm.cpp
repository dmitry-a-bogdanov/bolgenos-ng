#include <bolgenos-ng/asm.hpp>

lib::ostream& x86::operator <<(lib::ostream& out,
		const x86::registers_dump_t& regs) {
	lib::scoped_format_guard format_guard(out);

	out	<< lib::setw(0) << lib::hex << lib::setfill(' ');
	out	<< " eax = "
			<< lib::setw(8) << lib::setfill('0')
				<< regs.eax
			<< lib::setfill(' ') << lib::setw(0)
		<< ' '
		<< " ebx = " << lib::setw(8) << regs.ebx << lib::setw(0) << ' '
		<< " ecx = " << lib::setw(8) << regs.ecx << lib::setw(0) << ' '
		<< " edx = " << lib::setw(8) << regs.edx << lib::setw(0) << ' '
		<< lib::endl
		<< " esi = " << lib::setw(8) << regs.esi << lib::setw(0) << ' '
		<< " edi = " << lib::setw(8) << regs.edi << lib::setw(0) << ' '
		<< " ebp = " << lib::setw(8) << regs.ebp << lib::setw(0) << ' '
		<< " esp = " << lib::setw(8) << regs.esp << lib::setw(0) << ' ';

	return out;
}
