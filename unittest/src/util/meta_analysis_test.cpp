#include <util/meta_analysis.h>

#include <gtest/gtest.h>

#include "types.h"

namespace mixer {

TEST(meta_analysis, p_value_zero) {
	const statistic_analysis m(statistic(statistic_type::chi2, 0, 0, 10));

	EXPECT_FALSE(m.pass());
	EXPECT_TRUE(m.has_suspicion());
	EXPECT_TRUE(m.has_remark());
	EXPECT_EQ(m.get_failure_strength(), 10);
	EXPECT_EQ(m.to_string(), "failure(10)");
}

TEST(meta_analysis, p_value_01) {
	const statistic_analysis m(statistic(statistic_type::chi2, 0, 0.1, 10));

	EXPECT_TRUE(m.pass());
	EXPECT_FALSE(m.has_suspicion());
	EXPECT_TRUE(m.has_remark());
	EXPECT_EQ(m.get_failure_strength(), 1);
	EXPECT_EQ(m.to_string(), "minor(1)");
}

TEST(meta_analysis, p_value_001) {
	const statistic_analysis m(statistic(statistic_type::chi2, 0, 0.01, 10));

	EXPECT_TRUE(m.pass());
	EXPECT_FALSE(m.has_suspicion());
	EXPECT_TRUE(m.has_remark());
	EXPECT_EQ(m.get_failure_strength(), 2);
	EXPECT_EQ(m.to_string(), "minor(2)");
}

TEST(meta_analysis, p_value_0002321) {
	const statistic_analysis m(statistic(statistic_type::chi2, 0, 0.002321, 10));

	EXPECT_TRUE(m.pass());
	EXPECT_FALSE(m.has_suspicion());
	EXPECT_TRUE(m.has_remark());
	EXPECT_EQ(m.get_failure_strength(), 2);
	EXPECT_EQ(m.to_string(), "minor(2)");
}


}
