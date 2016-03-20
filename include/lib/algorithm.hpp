#pragma once

/// \brief Library namespace.
///
/// Namespace that combines STL-like functionality.
namespace lib {


/// \brief Iterate through container
///
/// \tparam ForwardIterator type of iterator that points to the beginning
/// of container.
/// \tparam EndIterator type of iterator that points to the end of container.
/// \tparam Function type of function to be called for every element
/// of container.
/// \param first iterator that points to the beginning of container.
/// \param last iterator that points to the end of container.
/// \param func function to be called for every element of container.
template<typename ForwardIterator, typename EndIterator,
		typename Function>
void for_each(ForwardIterator first, EndIterator last, Function func) {
	for(; first != last; ++first)
		func(*first);
}


} // namespace lib
