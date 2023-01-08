#include <statistics/bit_correlation.h>

#include <gtest/gtest.h>

#include "testutil.h"
#include "prngs32.h"

namespace mixer {

TEST(bit_correlation, bit_count_no_change) {
	using T = uint64_t;
	const auto rs = bit_count_correlation_test(10000, test_stream<T>());
	EXPECT_EQ(rs.size(), 19);
	double p_sum = 0;
	double s_sum = 0;
	for (const auto& r : rs) {
		p_sum += r.stats->p_value;
		s_sum += r.stats->value;
	}
	EXPECT_NEAR(p_sum, 6.9050525, 1e-4);
	EXPECT_NEAR(s_sum, 347.34883, 1e-4);
}

}
