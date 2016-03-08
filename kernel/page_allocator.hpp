#pragma once


#include <bolgenos-ng/bitarray>


namespace memory {


namespace allocators {

template<size_t MaxOrder>
class BuddyAllocator;

class MemoryRegion;

class PageAllocator {
public:
	struct buddy_order {
		enum { value = 10 };
	};

	PageAllocator() = default;
	PageAllocator(const PageAllocator &) = delete;
	PageAllocator& operator =(const PageAllocator &) = delete;
	~PageAllocator() {}

	void initialize(BuddyAllocator<buddy_order::value> *primary,
			page_frame_t *first_free);

	void *allocate(size_t pages);

	void deallocate(void *memory);

private:
	BuddyAllocator<buddy_order::value> *primary_ = nullptr;
	util::inplace::BitArray map_;
};


} // namespace allocators

} // namespace memory
