#pragma once

#include <cstddef.hpp>

#include "bitarray.hpp"


namespace memory {


namespace allocators {


/// \brief Slab allocation area descriptor.
///
/// The structure holds info for slab allocator.
class SlabAllocator {
public:

	/// \brief Initialize slab.
	///
	/// The function allocates memory, setups allocation map and fills
	/// provided slab area descriptor.
	///
	/// \param elem_size Size of element in slab area.
	/// \param nelems Number of elemnts in slab area.
	SlabAllocator(size_t elem_size, size_t nelems);


	/// Default constructor.
	SlabAllocator() = default;


	/// Copy-constructing is denied.
	SlabAllocator(const SlabAllocator&) = delete;


	/// Copy-assignment is denied.
	SlabAllocator& operator =(const SlabAllocator&) = delete;


	/// \brief Initialize slab
	///
	/// The functions initializes SlabAllocator according to specified
	/// parameters.
	///
	/// \param elem_size Size of one elements in slab.
	/// \param nelems Number of elements that slab should operate.
	/// \return Boolean status of allocator. True if initialization was
	/// successful, false - otherwise.
	bool initialize(size_t elem_size, size_t nelems);


	/// \brief Allocate element from slab.
	///
	/// The function allocates one element from slab if possible.
	///
	/// \return Pointer to allocated memory or nullptr.
	void *allocate();


	/// \brief Free allocated element.
	///
	/// The function frees specified element in the given slab area.
	///
	/// \param addr Previously allocated element from this slab area.
	void deallocate(void *addr);


	/// \brief Get initialization status of \ref SlabAllocator.
	///
	/// The function returns initialization status of \ref SlabAllocator.
	///
	/// \return true if OK, false if error has occurred.
	bool is_initialized() const;


	/// \brief Check that memory is owned by allocator.
	///
	/// The function checks that the specified memory is owned by the
	/// given allocator.
	///
	/// \param memory Pointer to memory for checking.
	/// \return true if the specified memory is owned by the allocator;
	/// false otherwise.
	bool owns(void *memory) const;


	/// \brief \ref SlabAllocator destructor.
	///
	/// The functions destructs \ref SlabAllocator object and frees memory
	/// related to this allocator.
	~SlabAllocator();


private:

	/// \brief Get allocation status of memory unit.
	///
	/// The function gets allocation status of specified memory unit
	/// in slab area.
	/// \param index Index of memory unit in slab.
	/// \return true if unit is free, false otherwise.
	bool is_free(size_t index);


	/// \brief Set allocation status of memory unit.
	///
	/// The function sets boolean value of allocation status
	/// to specified memory unit in the slab area.
	/// \param index Index of memory unit in slab.
	/// \param free Memory allocation status to be set. True if free,
	/// false otherwise.
	void set_free(size_t index, bool free);


	/// Size of element in size slab.
	size_t elem_size_ = 0;


	/// Number of elements in slab.
	size_t nelems_ = 0;


	/// Pointer to memory of slab.
	void *memory_ = nullptr;


	/// Allocated memory area.
	void *area_ = nullptr;


	/// Allocation status of elements.
	util::inplace::BitArray allocation_map_ = {};


	/// Initialization was successful.
	bool initialized_ = false;
};


} // namespace allocators


} // namespace memory
