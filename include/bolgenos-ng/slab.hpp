#pragma once

#include <bolgenos-ng/bitarray.hpp>
#include <bolgenos-ng/stdtypes.hpp>


namespace memory {


namespace allocators {


/**
* \brief Slab allocation area descriptor.
*
* The structure holds info for slab allocator.
*/
class SlabAllocator {
public:
	/**
	* \brief Initialize slab.
	*
	* The function allocates memory, setups allocation map and fills provided
	* slab area descriptor.
	*
	* \param elem_size Size of element in slab area.
	* \param nelems Number of elemnts in slab area.
	*/
	SlabAllocator(size_t elem_size, size_t nelems);


	/// Default constructing is denied.
	SlabAllocator() = delete;


	/// Copy-constructing is denied.
	SlabAllocator(const SlabAllocator&) = delete;


	/// Copy-assignment is denied.
	SlabAllocator& operator =(const SlabAllocator&) = delete;


	/**
	* \brief Allocate element from slab.
	*
	* The function allocates one element from slab if possible.
	*
	* \return Pointer to allocated memory or nullptr.
	*/
	void *allocate();


	/**
	* \brief Free allocated element.
	*
	* The function frees specified element in the given slab area.
	*
	* \param addr Previously allocated element from this slab area.
	*/
	void deallocate(void *addr);


	/**
	* \brief Get initialization status of \ref SlabAllocator.
	*
	* The function returns initialization status of \ref SlabAllocator.
	*
	* \return true if ok, false if error has occured.
	*/
	bool initialized() const;


	/**
	* \brief \ref SlabAllocator destructor.
	*
	* The functions destructs \ref SlabAllocator object and frees memory
	* related to this allocator.
	*/
	~SlabAllocator();

protected:
	/**
	* \brief Get allocation status of memory unit.
	*
	* The function gets allocation status of specified memory unit
	* in slab area.
	* \param index Index of memory unit in slab.
e	* \return true if unit is free, false otherwise.
	*/
	bool is_free(size_t index);


	/**
	* \brief Set allocation status of memory unit.
	*
	* The function sets boolean value of allocation status
	* to specified memory unit in the slab area.
	* \param index Index of memory unit in slab.
	* \param free Memory allocation status to be set. True if free,
	* false otherwise.
	*/
	void set_free(size_t index, bool free);

private:

	/// Size of element in size slab.
	size_t elem_size_;


	/// Number of elements in slab.
	size_t nelems_;


	/// Pointer to memory of slab.
	void *memory_;


	/// Allocated memory area.
	void *area_;


	/// Allocation status of elements.
	util::inplace::BitArray allocation_map_;


	/// Initialization was successed.
	bool initialized_;
};


} // namespace allocators


} // namespace memory
