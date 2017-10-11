#pragma once

#include <cstddef>


namespace memory {

constexpr inline std::size_t PAGE_SIZE = 4096;

/// \brief Page in real memory.
///
/// Used only for simplifying address arithmetics.
struct __attribute__((packed)) page_frame_t {
	std::byte bytes[PAGE_SIZE]; ///< bytes in real page frame
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
