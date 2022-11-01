#include <statistics/waldwolfowitz.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(waldwolfowitz, empty) {
	const auto ww = wald_wolfowitz({});
	EXPECT_EQ(ww.runs, 0);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, one_element) {
	const auto ww = wald_wolfowitz({1});
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, two_same) {
	const auto ww = wald_wolfowitz({1, 1});
	EXPECT_EQ(ww.runs, 1);
	EXPECT_EQ(ww.n_plus, 0);
	EXPECT_EQ(ww.n_minus, 0);
}

TEST(waldwolfowitz, two_different) {
	const auto ww = wald_wolfowitz({1, 2});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 1);
	EXPECT_EQ(ww.n_minus, 1);
}


TEST(waldwolfowitz, two_runs) {
	const auto ww = wald_wolfowitz({1, 1, 1, 2, 2, 2});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 3);
	EXPECT_EQ(ww.n_minus, 3);
}

TEST(waldwolfowitz, two_runs_2) {
	const auto ww = wald_wolfowitz({3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1});
	EXPECT_EQ(ww.runs, 2);
	EXPECT_EQ(ww.n_plus, 3);
	EXPECT_EQ(ww.n_minus, 5);
}


}
