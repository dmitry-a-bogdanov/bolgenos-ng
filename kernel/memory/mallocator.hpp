#pragma once

#include <bolgenos-ng/slab.hpp>

#include "page_allocator.hpp"


namespace memory {


namespace allocators {


class Mallocator {
public:
	Mallocator() = default;
	Mallocator(const Mallocator &) = delete;
	Mallocator& operator =(const Mallocator &) = delete;
	~Mallocator() = default;
	void initialize(PageAllocator *fallback,
			size_t chain_memory, size_t chain_length);
	void *allocate(size_t bytes);
	void deallocate(void *memory);
private:
	void assert_initialized();

	size_t chain_length_ = 0;
	/// Chain of slab allocators.
	SlabAllocator *chain_ = nullptr;
	PageAllocator *fallback_ = nullptr;
	SlabAllocator internal_allocator_ = {};
	bool _initialized{false};
};



} // namespace allocators


} // namespace memory
