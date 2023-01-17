#include <statistics/bit_correlation.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(bit_correlation, bit_count_2d_no_change) {
	using T = uint64_t;
	const auto rs = bit_count_2d_test(10000, test_stream<T>());
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

TEST(bit_correlation, bit_count_3d_no_change) {
	using T = uint32_t;
	const auto rs = bit_count_3d_test(1ull << 20, test_stream<T>());
	EXPECT_EQ(rs.size(), 241);
	double p_sum = 0;
	double s_sum = 0;
	for (const auto& r : rs) {
		p_sum += r.stats->p_value;
		s_sum += r.stats->value;
	}
	EXPECT_NEAR(p_sum, 97.8380, 1e-4);
	EXPECT_NEAR(s_sum, 2896.5008, 1e-4);
}


// TEST(bit_correlation, bit_count_3d_fail_with_expected_threashold_five) {
// 	using T = uint32_t;
// 	const auto rs = bit_count_3d_test(1ull << 13, create_trng_stream<T>(224327680));
// 	EXPECT_EQ(rs.size(), 89);
// }


}
