#include <statistics/bit_correlation.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {
TEST(bit_correlation, bit_count_2d_no_change) {
	using T = uint64_t;
	const auto rs = bit_count_2d_test(1ull << 20, test_stream<T>());
	EXPECT_EQ(rs.size(), 23);
	double p_sum = 0;
	double s_sum = 0;
	for (const auto& r : rs) {
		p_sum += r.stats->p_value;
		s_sum += r.stats->value;
	}
	EXPECT_NEAR(p_sum, 11.2830, 1e-4);
	EXPECT_NEAR(s_sum, 639.6820, 1e-4);
}
}
