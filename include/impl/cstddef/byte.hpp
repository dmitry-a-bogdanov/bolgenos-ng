#pragma once

namespace std {


enum class byte: unsigned char
{
};


template<typename Integer>
constexpr Integer to_integer(std::byte b) noexcept
{
	return static_cast<Integer>(b);
}


template<typename Integer>
constexpr std::byte operator<<(std::byte b, Integer shift)
{
	return std::byte{static_cast<unsigned char>(b) << shift};
}


template<typename Integer>
constexpr std::byte& operator<<=(std::byte b, Integer shift)
{
	return b = std::byte{static_cast<unsigned char>(b) << shift};
}


template<typename Integer>
constexpr std::byte operator>>(std::byte b, Integer shift)
{
	return std::byte{static_cast<unsigned char>(b) >> shift};
}


template<typename Integer>
constexpr std::byte& operator>>=(std::byte b, Integer shift)
{
	return b = std::byte{static_cast<unsigned char>(b) >> shift};
}


constexpr std::byte operator|(std::byte lhs, std::byte rhs)
{
	return std::byte(static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs));
}


constexpr std::byte operator&(std::byte lhs, std::byte rhs)
{
	return std::byte(static_cast<unsigned char>(lhs) & static_cast<unsigned char>(rhs));
}


constexpr std::byte operator^(std::byte lhs, std::byte rhs)
{
	return std::byte(static_cast<unsigned char>(lhs) ^ static_cast<unsigned char>(rhs));
}


constexpr std::byte operator~(std::byte lhs)
{
	return std::byte(static_cast<unsigned char>(lhs));
}


} // namespace std
