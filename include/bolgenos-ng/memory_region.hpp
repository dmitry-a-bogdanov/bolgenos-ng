#pragma once

#include <bolgenos-ng/stdtypes>

#include <bolgenos-ng/page.hpp>


namespace memory {


/// \brief Memory region descriptor.
///
/// Structure describes memory regions like high and low memory.
class MemoryRegion {
public:

	inline void begin(page_frame_t *address) {
		begin_ = address;
	}


	inline page_frame_t *begin() const {
		return begin_;
	}


	inline void end(page_frame_t *address) {
		end_ = address;
	}


	inline page_frame_t *end() const {
		return end_;
	}


	/// \brief Check that page frame is owned by the memory region.
	///
	/// The fuction checks whether specified page frame is owned by
	/// the memory region.
	///
	/// \param frame Specified frame to be checked for ownership.
	/// \return true if specified frame is owned by memory region.
	inline bool owns(const page_frame_t *frame) const {
		return (begin_ <= frame) && (frame < end_);
	}

	/// \brief Get page frame index.
	///
	/// Get index of specified page frame in memory region.
	///
	/// \param frame Specified page frame.
	/// \return Index of specified page frame.
	inline ptrdiff_t index_of(const page_frame_t *frame) const {
		return frame - begin_;
	}


	/// \brief Size of the memory region.
	///
	/// The function returns size of the memory region.
	///
	/// \return Size of memory region in page frames.
	inline size_t size() const {
		return end_ - begin_;
	}


protected:


	/// \brief Begin of the memory region.
	///
	/// Pointer to the first page frame in memory region.
	page_frame_t *begin_ = nullptr;


	/// \brief End of the memory region.
	///
	/// Pointer to the first page frame that is not owned by the memory
	/// region.
	page_frame_t *end_ = nullptr;
};



} // namespace memory
