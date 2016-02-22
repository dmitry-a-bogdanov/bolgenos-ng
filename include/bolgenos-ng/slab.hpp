#pragma once

#include <bolgenos-ng/int_types.h>

/**
* \brief Slab allocation area descriptor.
*
* The structure holds info for slab allocator.
*/
class slab_area {
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
	slab_area(size_t elem_size, size_t nelems);

	slab_area() = delete;
	slab_area(const slab_area&) = delete;

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
	* \brief Get initialization status of \ref slab_area.
	*
	* The function returns initialization status of \ref slab_area.
	*
	* \return true if ok, false if error has occured.
	*/
	inline bool initialized() { return _initialized; }

	/**
	* \brief \ref slab_area destructor.
	*
	* The functions destructs \ref slab_area object and frees memory
	* related to this allocator.
	*/
	~slab_area();

protected:
	/**
	* \brief Get allocation status of memory unit.
	*
	* The function gets allocation status of specified memory unit in slab area.
	* \param index Index of memory unit in slab.
	* \return Allocation status of memory unit (\ref MEM_FREE or \ref MEM_USED).
	*/
	int get_status(size_t index);


	/**
	* \brief Set allocation status of memory unit.
	*
	* The function sets given allocation status to specified memory unit
	* in slab area.
	* \param index Index of memory unit in slab.
	* \param status Memory allocation status to be set (\ref MEM_FREE
	*	or \ref MEM_USED)
	*/
	void set_status(size_t index, int status);
private:
	size_t _elem_size;		/*!< Size of element in size slab. */
	size_t _nelems;			/*!< Number of elements in slab. */
	void *_memory;			/*!< Pointer to memory of slab. */
	uint8_t *_allocation_map;	/*!< Allocation status of elements. */
	bool _initialized;		/*!< Initilization was successed. */
};

