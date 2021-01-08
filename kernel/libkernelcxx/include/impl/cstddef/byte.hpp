#pragma once

namespace lib {


enum class byte: unsigned char
{
};


template<typename Integer>
constexpr Integer to_integer(byte b) noexcept
{
	return static_cast<Integer>(b);
}


template<typename Integer>
constexpr byte operator<<(byte b, Integer shift)
{
	return byte{static_cast<unsigned char>(b) << shift};
}


template<typename Integer>
constexpr byte& operator<<=(byte b, Integer shift)
{
	return b = byte{static_cast<unsigned char>(b) << shift};
}


template<typename Integer>
constexpr byte operator>>(byte b, Integer shift)
{
	return byte{static_cast<unsigned char>(b) >> shift};
}


template<typename Integer>
constexpr byte& operator>>=(byte b, Integer shift)
{
	return b = byte{static_cast<unsigned char>(b) >> shift};
}


constexpr byte operator|(byte lhs, byte rhs)
{
	return byte(static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs));
}


constexpr byte operator&(byte lhs, byte rhs)
{
	return byte(static_cast<unsigned char>(lhs) & static_cast<unsigned char>(rhs));
}


constexpr byte operator^(byte lhs, byte rhs)
{
	return byte(static_cast<unsigned char>(lhs) ^ static_cast<unsigned char>(rhs));
}


constexpr byte operator~(byte lhs)
{
	return byte(static_cast<unsigned char>(lhs));
}


} // namespace lib
