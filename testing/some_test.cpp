#include <iostream>
#include <algorithm>

/**
 * Literal class type that wraps a constant expression string.
 *
 * Uses implicit conversion to allow templates to *seemingly* accept constant strings.
 */
 /*
template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    constexpr StringLiteral(const StringLiteral<N>&) = default;
    constexpr StringLiteral(StringLiteral<N>&&) = default;


	char value[N];
};

template<StringLiteral lit>
void Print() {
    // The size of the string is available as a constant expression.
    constexpr auto size = sizeof(lit.value);

    // and so is the string's content.
    constexpr auto contents = lit.value;

    std::cout << "Size: " << size << ", Contents: " << contents << std::endl;
}


template<StringLiteral lit>
struct C1 {
    	C1() {
    		Print<lit>();
    	}
};

template<StringLiteral lit>
struct C2:
	C1<lit.value>
{};
*/

template<class Char, Char ...Chars>
struct basic_static_string {
	consteval basic_static_string(const basic_static_string<Char, Chars...>&) noexcept = default;
	consteval basic_static_string(basic_static_string<Char, Chars...>&&)  noexcept = default;
	consteval basic_static_string() noexcept = default;
	constexpr ~basic_static_string() noexcept = default;

	constexpr basic_static_string declcopy() const {
		return basic_static_string{};
	}
	static constexpr Char value[sizeof...(Chars) + 1]{Chars..., '\0'};
	using type = basic_static_string<Char, Chars...>;
};

template<class Char, Char ...Chars, class Char2, Char2 ... Chars2>
constexpr basic_static_string<Char2, Chars2...> f(const basic_static_string<Char, Chars2...>& bss) {
	return bss;
}


template<class Char, Char ...Chars>
constexpr basic_static_string<Char, Chars...> operator""_ss() {
	return basic_static_string<Char, Chars...>{};
}

template<const basic_static_string lit>
struct C1 {

};

template<basic_static_string lit>
struct C2
	: C1<lit> 	{

};


int main()
{
    C1<"test"_ss> c1;
    C2<"test2"_ss> c2;
}