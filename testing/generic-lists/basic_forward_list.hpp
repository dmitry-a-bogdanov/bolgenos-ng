#pragma once

#include <gtest/gtest.h>

#include <lib/type_traits.hpp>

namespace testing {


namespace ____impl {

template<class N>
struct has_member_next
{
private:
	template<typename T>
	static lib::true_type test(decltype(T::next)*) {}

	template<typename>
	static lib::false_type test(...) {}

public:
	static constexpr bool value = decltype(test<N>(nullptr))::value;
};

template<class N>
constexpr bool has_member_next<N>::value;


};

/*
template<class Node>
class basic_forward_list {
public:
	using node_type = Node;

	static_assert(____impl::has_member_next<node_type>::value,
		"Node type argument must have public member named 'next'");
}; // class basic_forward_list
*/

} // namespace testing
