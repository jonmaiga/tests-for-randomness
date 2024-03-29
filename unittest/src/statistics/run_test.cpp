#include <statistics/run.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {
TEST(run, empty) {
	using V = std::vector<uint64_t>;
	const auto ww = generate_runs_data<V>({}, 0);
	EXPECT_EQ(ww.runs, 0);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(run, one_element) {
	using V = std::vector<uint64_t>;
	const auto ww = generate_runs_data<V>({1}, 1);
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(run, two_same) {
	using V = std::vector<uint64_t>;
	const auto ww = generate_runs_data<V>({1, 1}, 1);
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(run, two_different) {
	using V = std::vector<double>;
	const auto ww = generate_runs_data<V>({1., 2}, 1.5);
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 1);
	EXPECT_EQ(ww.n_minus, 1);
}

TEST(run, two_runs) {
	using V = std::vector<uint64_t>;
	const auto ww = generate_runs_data<V>({1, 1, 1, 2, 3, 3, 3}, 2);
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 3);
	EXPECT_EQ(ww.n_minus, 3);
}

TEST(run, two_runs_2) {
	using V = std::vector<double>;
	const auto ww = generate_runs_data<V>({3., 3, 3, 2, 2, 2, 1, 1, 1, 1, 1}, 1.8182);
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 6);
	EXPECT_EQ(ww.n_minus, 5);
	EXPECT_NEAR(runs_stats(ww)->p_value, 0.002724, 1e-4);
}

TEST(run, no_change) {
	const auto r = runs_test(1 << 15, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 0.724030, 1e-4);
	EXPECT_NEAR(r->p_value, 0.469046, 1e-4);
}
}
