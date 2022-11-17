#include <test/permutation.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace mixer {

TEST(permutation, no_change) {
	const auto r = permutation_test(1000, test_stream());
	EXPECT_EQ(r.size(), 1);
	EXPECT_NEAR(r.front().value, 36.3470, 1e-4);
	EXPECT_NEAR(r.front().p_value, 0.2335, 1e-4);
}

}
