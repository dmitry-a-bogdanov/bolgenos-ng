#pragma once

#include <cstddef>
#include <bolgenos_config.hpp>


namespace memory {


/// \brief Page in real memory.
///
/// Used only for simplifying address arithmetics.
struct __attribute__((packed)) page_frame_t {
	std::byte bytes[config::PAGE_SIZE]; ///< bytes in real page frame
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
