#pragma once

#include <bolgenos-ng/stdtypes>

#include "config.h"


namespace memory {


/// \brief Page in real memory.
///
/// Used only for simplifying address arithmetics.
struct __attribute__((packed)) page_frame_t {
	uint8_t bytes[PAGE_SIZE]; ///< bytes in real page frame
};
static_assert(sizeof(page_frame_t) == PAGE_SIZE, "Wrong size of page_frame_t");


/// \brief Page descriptor structure.
///
/// The structure is intended to describe page frame.
struct __attribute__((packed)) page_t {
	/// Next page in allocation block.
	page_t *next;


	/// Page is free flag.
	bool free;
};


} // namespace memory
