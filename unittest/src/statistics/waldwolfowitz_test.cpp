#include <statistics/waldwolfowitz.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(waldwolfowitz, empty) {
	const auto ww = generate_wald_wolfowitz_data<uint64_t>({});
	EXPECT_EQ(ww.runs, 0);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, one_element) {
	const auto ww = generate_wald_wolfowitz_data<uint64_t>({1});
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, two_same) {
	const auto ww = generate_wald_wolfowitz_data<uint64_t>({1, 1});
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, two_different) {
	const auto ww = generate_wald_wolfowitz_data<double>({1, 2});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 1);
	EXPECT_EQ(ww.n_minus, 1);
}

TEST(waldwolfowitz, two_runs) {
	const auto ww = generate_wald_wolfowitz_data<uint64_t>({1, 1, 1, 2, 2, 2});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 3);
	EXPECT_EQ(ww.n_minus, 3);
}

TEST(waldwolfowitz, two_runs_2) {
	const auto ww = generate_wald_wolfowitz_data<double>({3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 3);
	EXPECT_EQ(ww.n_minus, 5);
	EXPECT_NEAR(wald_wolfowitz_stats(ww)->p_value, 0.01543, 1e-4);
}

TEST(waldwolfowitz, no_change) {
	const auto r = wald_wolfowitz_test(50, test_stream()).front().stats;
	EXPECT_NEAR(r->value, -0.5855, 1e-4);
	EXPECT_NEAR(r->p_value, 0.5581, 1e-4);
}

}
