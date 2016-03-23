#pragma once

/// \brief Library namespace.
///
/// Namespace that combines STL-like functionality.
namespace lib {


/// \brief Iterate through container
///
/// \tparam ForwardIterator type of iterator that points to the beginning
/// of container.
/// \tparam Sentinel type of iterator that points to the end of container.
/// \tparam Function type of function to be called for every element
/// of container.
/// \param first iterator that points to the beginning of container.
/// \param last iterator that points to the end of container.
/// \param func function to be called for every element of container.
template<typename ForwardIterator, typename Sentinel, typename Function>
void for_each(ForwardIterator first, Sentinel last, Function func) {
	for(; first != last; ++first)
		func(*first);
}


template<typename ForwardIterator, typename Sentinel, typename Function>
ForwardIterator find_if(ForwardIterator first, Sentinel last, Function func) {
	for (; first != last; ++first) {
		if (func(*first)) {
			break;
		}
	}
	return first;
}


} // namespace lib
