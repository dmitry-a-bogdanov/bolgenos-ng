#pragma once

#include <bolgenos-ng/error.h>
#include "new.hpp"
#include "type_traits.hpp"

namespace lib {

template<typename T>
class optional {
public:
	using value_type = T;

	constexpr optional() = default;

	template<class U = T>
	constexpr optional(U&& u) {
		new (&_storage) T(lib::forward<U>(u));
		_has_value = true;
	}

	constexpr optional(optional&& other) {
		if (other.has_value()) {
			new (&_storage) T(lib::move(other.value()));
		}
	}

	constexpr optional& operator=(optional&& other) {
		if (has_value()) {
			value().T::~T();
		}
		new (&_storage) T(lib::move(other.value()));
		_has_value = true;
		return *this;
	}

	template<class U = T>
	constexpr optional& operator=(U&& u) {
		if (has_value()) {
			value().T::~T();
		}
		new (&_storage) T(lib::forward<U>(u));
		_has_value = true;
		return *this;
	}

	constexpr bool has_value() const { return _has_value; }
	constexpr explicit operator bool() const { return has_value(); }

	constexpr T& value() & {
		assert_has_value();
		return *ptr();
	}

	constexpr const T& value() const & {
		assert_has_value();
		return *ptr();
	}

	constexpr T&& value() && {
		assert_has_value();
		return lib::move(*ptr());
	}

	constexpr const T&& value() const && {
		return lib::move(*ptr());
	}

	~optional() {
		if (has_value()) {
			value().T::~T();
		}
	}

private:
	constexpr T* ptr() {
		return reinterpret_cast<T*>(&_storage);
	}

	constexpr const T* ptr() const {
		return reinterpret_cast<const T*>(&_storage);
	}

	void assert_has_value() const {
		if (!has_value()) {
			::panic("bad_optional_access");
		}
	}

	lib::aligned_storage_t<sizeof(T), alignof(T)> _storage{};
	bool _has_value{false};
};

}
