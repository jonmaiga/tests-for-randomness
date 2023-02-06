#include <statistics/fishersmethod.h>

#include <gtest/gtest.h>

namespace tfr {

TEST(fishersmethod, basic) {
	using T = std::vector<double>;
	EXPECT_NEAR(fishers_combined_probabilities<T>({1.}), 1, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities<T>({0.}), 0, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities<T>({0., 1.}), 0, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities<T>({0.5}), 0.5, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities<T>({0.5, 0.5}), 0.5965, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities<T>({0.1, 0.5, 1.}), 0.4241, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities<T>({0.,1,1,1,1,1,1,1,1,1,1,1}), 5.9719e-07, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities<T>({0.,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}), 0.0002135, 1e-4);
}

}
