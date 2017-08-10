#pragma once

#include "types.hpp"

namespace bolgenos
{
namespace io
{
namespace vga
{


struct [[gnu::packed]] VideoMemory
{
	static constexpr std::size_t ROWS{ 25 };
	static constexpr std::size_t COLUMNS{ 80 };
	char_t video_buffer[ROWS][COLUMNS];
};


static_assert(sizeof(VideoMemory) == 2*25*80);



}
}
}
