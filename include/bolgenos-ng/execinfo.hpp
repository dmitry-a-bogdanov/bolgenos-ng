#pragma once

#include <ostream>


namespace execinfo {


/// \brief Show backtrace.
///
/// The function prints backtrace starting from specified base pointer and
/// instruction pointer to specified ostream. If base and/or instruction pointer
/// is not specified corresponding value of caller's function is used.
///
/// \param out Stream for printing backtrace.
/// \param ebp Base pointer for backtracing.
/// \param eip Instruction pointer for backtracing.
void show_backtrace(std::ostream& out, void *ebp = nullptr,
		void *eip = nullptr);


} // namespace execinfo
