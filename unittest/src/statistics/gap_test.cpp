#include <statistics/gap.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {
std::vector<uint64_t> generate_gaps_upscale(uint64_t max_gap_size, double a, double b, const std::vector<double>& d) {
	return generate_gaps(max_gap_size, a, b, rescale_01_to_type(d));
}

TEST(gap, generate_gaps) {
	using T = std::vector<double>;
	using U = std::vector<uint64_t>;
	EXPECT_EQ(generate_gaps_upscale(2, 0, 1, {}), (U{0, 0}));

	EXPECT_EQ(generate_gaps_upscale(2, 0, 1, {0.}), (U{1, 0}));
	EXPECT_EQ(generate_gaps_upscale(2, 0, 1, {0.5}), (U{1, 0}));
	EXPECT_EQ(generate_gaps_upscale(2, 0, 1, {1.}), (U{1, 0}));

	EXPECT_EQ(generate_gaps_upscale(2, 0, .5, {0.}), (U{1, 0}));
	EXPECT_EQ(generate_gaps_upscale(2, 0, .5, {0.5}), (U{0, 0}));
	EXPECT_EQ(generate_gaps_upscale(2, 0, .5, {1.}), (U{0, 0}));

	EXPECT_EQ(generate_gaps_upscale(2, .5, 1, {0.}), (U{0, 0}));
	EXPECT_EQ(generate_gaps_upscale(2, .5, 1, {0.5}), (U{1, 0}));
	EXPECT_EQ(generate_gaps_upscale(2, .5, 1, {1.}), (U{1, 0}));

	EXPECT_EQ(generate_gaps_upscale(2, 0, 1, {0.25, 0.75}), (U{2, 0}));

	EXPECT_EQ(generate_gaps_upscale(2, 0, .5, {0.25, 0.75}), (U{1, 0}));
	EXPECT_EQ(generate_gaps_upscale(2, 0, .5, {0.75, 0.25}), (U{0, 1}));
	EXPECT_EQ(generate_gaps_upscale(2, 0, .5, {0.75, 0.25, 0.5, 0.25}), (U{0, 2}));
	EXPECT_EQ(generate_gaps_upscale(2, 0, .5, {0.25, 0.75, 0.25, 0.5, 0.25}), (U{1, 2}));
	EXPECT_EQ(generate_gaps_upscale(2, .5, .6, {0.25, 0.5}), (U{0, 1}));
}

TEST(gap, generate_too_large_gaps) {
	using T = std::vector<double>;
	using U = std::vector<uint64_t>;
	EXPECT_EQ(generate_gaps_upscale(2, .5, .6, {0.25, 0.35, 0.52, 0.25, 0.35, 0.52}), (U{0, 2}));
}

TEST(gap, generate_gap_probabilities) {
	const auto ps = generate_gap_probabilities(.1, .2);
	EXPECT_NEAR(ps[0], 0.1, 1e-4);
	EXPECT_NEAR(ps[1], 0.09, 1e-4);
	EXPECT_NEAR(ps[2], 0.081, 1e-4);
	EXPECT_NEAR(ps[3], 0.0729, 1e-4);
	EXPECT_NEAR(ps[4], 0.06561, 1e-4);
	EXPECT_NEAR(ps[5], 0.0590, 1e-4);
}

TEST(gap, no_change) {
	const auto r = gap_test<uint64_t>(1000, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 5.1234, 1e-4);
	EXPECT_NEAR(r->p_value, 0.4009, 1e-4);
}

TEST(gap, no_change_8) {
	auto n = 1 << 22;
	auto rs = gap_test<uint8_t>(n, test_stream_casted<uint8_t>(n));
	EXPECT_NEAR(rs.front().stats->value, 11.7348, 1e-4);
	EXPECT_NEAR(rs.front().stats->p_value, 0.1096, 1e-4);
	EXPECT_NEAR(rs.back().stats->value, 6.7704, 1e-4);
	EXPECT_NEAR(rs.back().stats->p_value, 0.4531, 1e-4);
}
}
