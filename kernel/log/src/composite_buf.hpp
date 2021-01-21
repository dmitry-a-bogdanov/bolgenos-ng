#pragma once

#include <concepts.hpp>
#include <optional.hpp>
#include <streambuf.hpp>
#include <utility.hpp>

template<lib::derived_from<lib::streambuf> A, lib::derived_from<lib::streambuf> B>
class CompositeBuf: public lib::streambuf {
public:
	CompositeBuf(A a, B b): _a(lib::move(a)), _b(lib::move(b)) {}
	CompositeBuf(A a): _a(lib::move(a)), _b{} {}
	CompositeBuf(B b): _a{}, _b(lib::move(b)) {}
	CompositeBuf(): _a{}, _b{} {}
	~CompositeBuf() = default;

	void set(B b) {
		_b = lib::move(b);
	}

protected:
	int overflow(int c) override {
		int result = c;
		if (_a.has_value()) {
			result = _a.value().sputc(c);
		}
		if (_b.has_value()) {
			result = _b.value().sputc(c);
		}
		return result;
	}

	size_t xsputn(const char *s, size_t n) override {
		size_t result = 0;
		if (_a.has_value()) {
			result = _a.value().sputn(s, n);
		}
		if (_b.has_value()) {
			result = _b.value().sputn(s, n);
		}
		return result;
	}

private:
	lib::optional<A> _a;
	lib::optional<B> _b;
};
