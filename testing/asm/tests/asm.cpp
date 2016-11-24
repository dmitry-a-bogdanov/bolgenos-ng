#include <gtest/gtest.h>

#include <bolgenos-ng/asm.hpp>

TEST(lzcnt, all)
{
	EXPECT_EQ(static_cast<uint32_t>(32), x86::lzcnt(0));
	for (uint32_t i = 0; i < 32; ++i) {
		EXPECT_EQ((32 - i - 1), x86::lzcnt(1 << i));
		EXPECT_EQ((32 - i - 1), x86::lzcnt((1 << i) | 1));
	}
}
