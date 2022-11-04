#include <statistics/waldwolfowitz.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(waldwolfowitz, empty) {
	const auto ww = wald_wolfowitz_stats({});
	EXPECT_EQ(ww.runs, 0);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, one_element) {
	const auto ww = wald_wolfowitz_stats({1});
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, two_same) {
	const auto ww = wald_wolfowitz_stats({1, 1});
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, two_different) {
	const auto ww = wald_wolfowitz_stats({1, 2});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 1);
	EXPECT_EQ(ww.n_minus, 1);
}

TEST(waldwolfowitz, two_runs) {
	const auto ww = wald_wolfowitz_stats({1, 1, 1, 2, 2, 2});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 3);
	EXPECT_EQ(ww.n_minus, 3);
}

TEST(waldwolfowitz, two_runs_2) {
	const auto ww = wald_wolfowitz_stats({3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 6);
	EXPECT_EQ(ww.n_minus, 5);
	EXPECT_NEAR(wald_wolfowitz_p_value(ww), 0.004266, 1e-4);
}

TEST(waldwolfowitz, no_change) {
	const auto r = wald_wolfowitz_test(50, test_stream());
	EXPECT_EQ(r.size(), 1);
	EXPECT_NEAR(r.front().value, 27, 1e-4);
	EXPECT_NEAR(r.front().p_value, 0.7659, 1e-4);
}

}
