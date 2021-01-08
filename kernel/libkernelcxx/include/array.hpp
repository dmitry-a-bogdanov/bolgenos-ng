#pragma once

#include <bolgenos-ng/error.h>

#include <utility.hpp>

namespace lib {

template<class T, size_t N>
class Array
{
public:
	constexpr T& operator[](size_t pos) noexcept { return _data[pos]; }
	constexpr const T& operator[](size_t pos) const noexcept { return _data[pos]; }

	template<size_t M>
	constexpr explicit Array(T values[M]) noexcept
		: _data{values}
	{}

	template<class... Ts>
	constexpr explicit Array(Ts&& ...values) noexcept
		: _data{lib::forward<Ts>(values)...} {}

	constexpr T& at(size_t pos) {
		if (pos >= N || pos < 0) [[unlikely]] {
			bug("index out of bounds");
		}
		return _data[pos];
	}

	constexpr const T& at(size_t pos) const {
		if (pos >= N || pos < 0) [[unlikely]] {
			bug("index out of bounds");
		}
		return _data[pos];
	}

	constexpr T* data() noexcept {
		return _data;
	}
	constexpr const T* data() const noexcept {
		return _data;
	}

private:
	T _data[N];
};

template<class T_, class... U>
Array(T_, U...) -> Array<T_, 1 + sizeof...(U)>;

}
