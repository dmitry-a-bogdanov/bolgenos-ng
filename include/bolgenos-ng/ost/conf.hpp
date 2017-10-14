#pragma once

#include <ostream>

namespace ost::Conf
{

std::ostream& bad_stream();
std::ostream& good_stream();
std::ostream& debug_stream();

void bad_stream(std::ostream& out);
void good_stream(std::ostream& out);
void debug_stream(std::ostream& out);

}
