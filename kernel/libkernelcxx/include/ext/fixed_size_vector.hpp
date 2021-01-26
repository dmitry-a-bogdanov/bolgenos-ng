#pragma once

#include <bolgenos-ng/error.h>
#include <log.hpp>
#include <algorithm.hpp>
#include <array.hpp>
#include <loggable.hpp>

namespace lib {

template<class T, size_t Size>
class FixedSizeVector: Loggable("fixed_size_vector")
{
public:
	constexpr FixedSizeVector() = default;

	void expand_by(size_t additional_elems) {
		expand_up_to(_size + additional_elems);
	}

	void expand_up_to(size_t new_size) {
		if (new_size > Size) [[unlikely]] {
			CRIT << "required size: " << new_size << " max size: " << Size << endl;
			panic("fault");
		}
		_size = lib::max(_size, new_size);
	}

	constexpr void push_back(T&& value) {
		expand_by(1);
		_data[_size - 1] = lib::move(value);
	}

	constexpr T& operator[](size_t pos) noexcept { return _data[pos]; }
	constexpr const T& operator[](size_t pos) const noexcept { return _data[pos]; }

	constexpr T& at(size_t pos) {
		if (pos >= _size || pos < 0) [[unlikely]] {
			CRIT << "out of bound. " << pos << " not in range 0-" << _size << endl;
			panic("fault");
		}
		return _data.at(pos);
	}

	constexpr const T& at(size_t pos) const {
		if (pos >= _size || pos < 0) [[unlikely]] {
			CRIT << "out of bound. " << pos << " not in range 0-" << _size << endl;
			panic("fault");
		}
		return _data.at(pos);
	}

	[[nodiscard]] constexpr size_t size() const noexcept {
		return _size;
	}

	constexpr const T* data() const noexcept {
		return _data.data();
	}

	constexpr T* data() noexcept {
		return _data.data();
	}

private:
	lib::array<T, Size> _data{};
	size_t _size{};
};

} // namespace lib
