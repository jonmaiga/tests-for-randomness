#include <rrc.h>
#include "testutil.h"

#include <gtest/gtest.h>

namespace tfr {
TEST(rrc, rrc_8) {
	using T = uint8_t;
	EXPECT_EQ(permute<T>(1, 0, rrc_type::identity), 1);
	EXPECT_EQ(permute<T>(1, 1, rrc_type::identity), 128);
	EXPECT_EQ(permute<T>(1, 7, rrc_type::identity), 2);

	EXPECT_EQ(permute<T>(1, 0, rrc_type::reverse), 128);
	EXPECT_EQ(permute<T>(1, 1, rrc_type::reverse), 64);
	EXPECT_EQ(permute<T>(1, 7, rrc_type::reverse), 1);

	EXPECT_EQ(permute<T>(1, 0, rrc_type::complement), 254);
	EXPECT_EQ(permute<T>(1, 1, rrc_type::complement), 127);
	EXPECT_EQ(permute<T>(1, 7, rrc_type::complement), 253);

	EXPECT_EQ(permute<T>(1, 0, rrc_type::reverse_complement), 127);
	EXPECT_EQ(permute<T>(1, 1, rrc_type::reverse_complement), 191);
	EXPECT_EQ(permute<T>(1, 7, rrc_type::reverse_complement), 254);
}
}
