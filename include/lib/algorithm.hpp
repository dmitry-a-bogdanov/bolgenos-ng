#pragma once

/// \brief Library namespace.
///
/// Namespace that combines STL-like functionality.
namespace lib {


/// \brief Iterate through range
///
/// Iterate through specified range and all specified function on every
/// element.
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
void for_each(ForwardIterator first, Sentinel last, Function func)
{
	for(; first != last; ++first)
		func(*first);
}


/// \brief Fill range with value.
///
/// Assign value to all elements in range [first, last).
///
/// \tparam ForwardIterator type of iterator that points to the beginning of
/// the range.
/// \tparam Sentinel type of iterator that points to the end of the range.
/// \tparam Type of value to be assigned.
/// \param first iterator that points to the benning of the range.
/// \param last iterator that points to the end of the range.
/// \param value value to be assinged to all elements in the range.
template<class ForwardIterator, class Sentinel, class T>
void fill(ForwardIterator first, Sentinel last, const T& value)
{
	while(first != last) {
		*first = value;
		++first;
	}
}


/// \brief Fill sequence with value
///
/// Assign value to the n elements of the sequence pointed by the first.
///
/// \tparam ForwardIterator type of iterator that points to the beginning of
/// the sequence.
/// \tparam Size type of size value.
/// \tparam Type of value to be assigned.
/// \param first iterator that points to the benning of the range.
/// \param n number of element to fill.
/// \param value value to be assinged to all elements in the range.
template<class ForwardIterator, class Size, class T>
void fill_n(ForwardIterator first, Size n, const T& value)
{
	while(n > 0) {
		*first = value;
		--n;
		++first;
	}
}


/// \brief Find element
///
/// Find first element in the range that satisfies specified predicate.
///
/// \tparam ForwardIterator type of iterator that points to the beginning
/// of container.
/// \tparam Sentinel type of iterator that points to the end of container.
/// \tparam Predicate type of predicate to be checked for every element.
/// \param first iterator that points to the beginning of container.
/// \param last iterator that points to the end of container.
/// \param pred condition to be checked for every element.
template<typename ForwardIterator, typename Sentinel, typename Predicate>
ForwardIterator find_if(ForwardIterator first, Sentinel last, Predicate pred)
{
	for (; first != last; ++first) {
		if (pred(*first)) {
			break;
		}
	}
	return first;
}


} // namespace lib
