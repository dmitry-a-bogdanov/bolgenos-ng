#pragma once

#include <cstddef.hpp>

#include "config.h"


namespace memory {


/// \brief Page in real memory.
///
/// Used only for simplifying address arithmetics.
struct __attribute__((packed, aligned(PAGE_SIZE))) page_frame_t {
	lib::byte bytes[PAGE_SIZE]; ///< bytes in real page frame
};


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
