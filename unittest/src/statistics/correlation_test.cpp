#include <statistics/correlation.h>

#include <gtest/gtest.h>

#include "mixers64.h"
#include "testutil.h"

namespace mixer {

TEST(pearson_correlation, unset) {
	EXPECT_FALSE(pearson_correlation_stats({},{}));
	EXPECT_FALSE(pearson_correlation_stats({1},{1}));
	EXPECT_FALSE(pearson_correlation_stats({1, 2},{1,2}));
}

TEST(pearson_correlation, basic) {
	EXPECT_NEAR(pearson_correlation_stats({1,2,3}, {1,2,3})->value, 1, 1e-4);
	EXPECT_NEAR(pearson_correlation_stats({3,2,1}, {3,2,1})->value, 1, 1e-4);
	EXPECT_NEAR(pearson_correlation_stats({1,2,3}, {3,2,1})->value, -1, 1e-4);
	EXPECT_NEAR(pearson_correlation_stats({1,2,3}, {1,2,4})->value, 0.9819, 1e-4);
	EXPECT_NEAR(pearson_correlation_stats({1,2,3,5}, {1,2,4,5})->value, 0.9621, 1e-4);
	EXPECT_NEAR(pearson_correlation_stats({1,2,3,5}, {5,4,2,1})->value, -0.9621, 1e-4);
}

TEST(pearson_correlation, p_value) {
	// same as mma
	EXPECT_NEAR(correlation_p_value(0.9861, 6), 0.0002862, 1e-4);
	EXPECT_NEAR(correlation_p_value(0.5727, 6), 0.2348, 1e-4);
	EXPECT_NEAR(correlation_p_value(-0.9621, 4), 0.03785, 1e-4);
	EXPECT_NEAR(correlation_p_value(0.9621, 4), 0.03785, 1e-4);
}

TEST(spearman_correlation, unset) {
	EXPECT_FALSE(spearman_correlation_stats({},{}));
	EXPECT_FALSE(spearman_correlation_stats({1},{1}));
	EXPECT_FALSE(spearman_correlation_stats({1, 2},{1,2}));
}

TEST(spearman_correlation, basic) {
	EXPECT_NEAR(spearman_correlation_stats({1,2,3}, {1,2,3})->value, 1, 1e-4);
	EXPECT_NEAR(spearman_correlation_stats({3,2,1}, {3,2,1})->value, 1, 1e-4);
	EXPECT_NEAR(spearman_correlation_stats({1,2,3}, {3,2,1})->value, -1, 1e-4);
	EXPECT_NEAR(spearman_correlation_stats(
		            {10,20,1,19,-5,0,-1,21},
		            {20,44,1,-11,-12,0,7,10})->value, -0.16666, 1e-4); // 0.5952 not agreeing with mma rank test (no regards to duplicates) 
}

TEST(spearman_correlation, p_value) {
	EXPECT_NEAR(correlation_p_value(0.5952, 6), 0.2126, 1e-4); // 0.1195 not agreeing with mma rank test
}

TEST(pearson_correlation, no_change) {
	const auto s = create_stream_from_mixer(test_stream(), mix64::mx3);
	const auto r = pearson_correlation_test(50, s).front().stats;
	EXPECT_NEAR(r->value, 0.09843, 1e-4);
	EXPECT_NEAR(r->p_value, 0.4964, 1e-4);
}

TEST(spearman_correlation, no_change) {
	const auto s = create_stream_from_mixer(test_stream(), mix64::mx3);
	const auto r = spearman_correlation_test(50, s).front().stats;
	EXPECT_NEAR(r->value, 0.03923, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7867, 1e-4);
}

TEST(kendall_correlation, no_change) {
	const auto s = create_stream_from_mixer(test_stream(), mix64::mx3);
	const auto r = kendall_correlation_test(50, s).front().stats;
	EXPECT_NEAR(r->value, 0.7444, 1e-4);
	EXPECT_NEAR(r->p_value, 0.4565, 1e-4);
}

}
