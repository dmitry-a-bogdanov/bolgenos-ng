#include <bolgenos-ng/page.hpp>

static_assert(sizeof(memory::page_frame_t) == PAGE_SIZE,
		"Wrong size of page_frame_t");
