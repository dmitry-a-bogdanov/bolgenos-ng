#pragma once

#include <utility.hpp>
#include <cstring.hpp>

namespace lib {


class string {
public:
	string() = default;

	string(string&) = delete;

	string(string&& other) noexcept {
		lib::swap(_string, other._string);
		lib::swap(_size, other._size);
		lib::swap(_capacity, other._capacity);
	}

	string& operator=(string&) = delete;

	string& operator=(string&& other) noexcept {
		lib::swap(_string, other._string);
		lib::swap(_size, other._size);
		lib::swap(_capacity, other._capacity);
		return *this;
	}

	~string() {
		delete[] _string;
	}

	string(const char * str) {
		_capacity = strlen(str);
		_string = new char[_capacity + 1];
		strncpy(_string, str, _capacity);
		_string[_capacity] = '\0';
		_size = _capacity;
	}

	[[nodiscard]] size_t size() const {
		return _size;
	}

	const char* c_str() const {
		return _string;
	}

private:
	size_t _capacity{0};
	size_t _size{0};
	char* _string{nullptr};
};

string operator+(const string& lhs, const string& rhs);

}

