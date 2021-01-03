#pragma once

#include <cstddef.hpp>
#include <cstdint.hpp>

#include "memory.hpp"


/// \brief Utilities
///
/// Namespace is a collection of data structures
namespace util {


/// \brief In-Place Data Structures
/// Namespace is a collection of data structures that can be inialized on
/// previously allocated memory. Such structures are collected into additional
/// namespace since they has different interface in comparison with
/// traditional dynamic allocation.
namespace inplace {


/// \brief Array of bits
///
/// Data structure that keeps and provides interface for array of bits.
class BitArray {
public:
	/// constructor.
	BitArray() = default;


	/// Copy-costructing is denied.
	BitArray(const BitArray &) = delete;


	/// Copy-assignment is denied.
	BitArray& operator =(const BitArray &) = delete;


	/// Descrutor.
	~BitArray() {}
	

	/// \brief Create BitArray.
	///
	/// The function initializes BitArray of specified size on specified
	/// memory with all bits set to zero/false.
	/// \param size Size of BitArray
	/// \param on_address Memory for initializing BitArray.
	void initialize(void *on_address, size_t size) {
		size_ = size;
		data_ = reinterpret_cast<decltype(data_)>(on_address);
		size_t bytes = memory::align_up<8>(size_) / 8;
		for (size_t idx = 0; idx != bytes; ++idx) {
			data_[idx] = 0x00;
		}
	}


	/// \brief Get element.
	///
	/// The function returns value of element with specified index.
	///
	/// \param index Index of interesting BitArray element.
	/// \return boolean value of element.
	inline bool get(size_t index) const {
		const size_t byte = index / 8;
		const size_t bit = index % 8;
		const uint8_t mask = uint8_t(1) << bit;
		return data_[byte] & mask;
	}


	/// \brief Set value of element.
	///
	/// The function sets boolean value of element with specified index.
	///
	/// \param index Index of element to be set.
	/// \param value Boolean value to be set.
	inline void set(size_t index, bool value) {
		const size_t byte = index / 8;
		const size_t bit = index % 8;
		const uint8_t mask = uint8_t(1) << bit;
		if (value) {
			data_[byte] |= mask;
		} else {
			data_[byte] &= ~mask;
		}
	}


	/// \brief Expected size of BitArray.
	///
	/// The function returns expected size of BitArray that can hold
	/// specified number of elements.
	///
	/// \param elems number of elements in interesting BitArray.
	/// \return Expected size of BitArray in bytes.
	static inline size_t expected_size(size_t elems) {
		const size_t bytes = elems / 8;
		const size_t bits = elems % 8;
		if (bits) {
			return bytes + 1;
		} else {
			return bytes;
		}
	}


private:
	/// Number of elements in BitArray.
	size_t size_ = 0;


	/// Pointer to memory that keeps BitArray.
	uint8_t *data_ = nullptr;
};


} // namespace inplace


} // namespace util
