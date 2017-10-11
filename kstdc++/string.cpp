#include "include/cstring"


size_t std::strlen(const char *str)
{
	size_t length = 0;
	while (*(str++))
		++length;
	return length;
}

char* std::strcpy(char *dest, const char *src)
{
	size_t pos = 0;
	do
	{
		dest[pos] = src[pos];
	} while(src[pos++]);
	return dest;
}

char* std::strncpy(char *dest, const char *src, size_t n)
{
	size_t pos = 0;
	while (pos != n && src[pos])
	{
		dest[pos] = src[pos];
		++pos;
	}
	while (pos != n)
	{
		dest[pos++] = '\0';
	}
	return dest;
}

