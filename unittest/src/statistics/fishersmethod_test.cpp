#include <statistics/fishersmethod.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(fishersmethod, basic) {
	EXPECT_NEAR(fishers_combined_probabilities({1}), 1, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities({0}), 0, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities({0, 1}), 0, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities({0.5}), 0.5, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities({0.5, 0.5}), 0.5965, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities({0.1, 0.5, 1.}), 0.4241, 1e-4);
	EXPECT_NEAR(fishers_combined_probabilities({0,1,1,1,1,1,1,1,1,1,1,1}), 0.01487, 1e-4);
}

}
