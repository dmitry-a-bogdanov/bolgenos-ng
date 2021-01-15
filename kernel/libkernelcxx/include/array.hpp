#pragma once

#include <bolgenos-ng/error.h>

#include <utility.hpp>

namespace lib {

template<class T, size_t N>
class array
{
public:
	template<typename ...Ts>
	constexpr array(Ts&& ...vals)
		: _data{forward<Ts>(vals)...}
	{}

	constexpr T& operator[](size_t pos) noexcept { return _data[pos]; }
	constexpr const T& operator[](size_t pos) const noexcept { return _data[pos]; }

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

	constexpr size_t size() const noexcept {
		return N;
	}

private:
	T _data[N];
};

template<class T_, class... U>
array(T_, U...) -> array<T_, 1 + sizeof...(U)>;

}
