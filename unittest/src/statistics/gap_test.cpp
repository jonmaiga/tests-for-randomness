#include <statistics/gap.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(gap, generate_gaps) {
	using T = std::vector<double>;
	using U = std::vector<uint64_t>;
	EXPECT_EQ(generate_gaps<T>(2, 0, 1, {}), (U{0, 0}));
	EXPECT_EQ(generate_gaps<T>(2, 0, 1, {0.5}), (U{1, 0}));
	EXPECT_EQ(generate_gaps<T>(2, 0, 1, {0.25, 0.75}), (U{2, 0}));

	EXPECT_EQ(generate_gaps<T>(2, 0, .5, {0.25, 0.75}), (U{1, 0}));
	EXPECT_EQ(generate_gaps<T>(2, 0, .5, {0.75, 0.25}), (U{0, 1}));
	EXPECT_EQ(generate_gaps<T>(2, 0, .5, {0.75, 0.25, 0.5, 0.25}), (U{0, 2}));
	EXPECT_EQ(generate_gaps<T>(2, 0, .5, {0.25, 0.75, 0.25, 0.5, 0.25}), (U{1, 2}));
	EXPECT_EQ(generate_gaps<T>(2, .5, .6, {0.25, 0.5}), (U{0, 1}));
}

TEST(gap, generate_too_large_gaps) {
	using T = std::vector<double>;
	using U = std::vector<uint64_t>;
	EXPECT_EQ(generate_gaps<T>(2, .5, .6, {0.25, 0.35, 0.52, 0.25, 0.35, 0.52}), (U{0, 2}));
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
	const auto r = create_gap_test(test_type::gap_low, 0, 0.33)(1000, test_stream());
	EXPECT_NEAR(r->value, 6.3867, 1e-4);
	EXPECT_NEAR(r->p_value, 0.8953, 1e-4);
}

}
