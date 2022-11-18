#include <statistics/permutation.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace mixer {

TEST(permutation, permutation_no_change) {
	const auto r = permutation_test(1000, test_stream());
	EXPECT_NEAR(r->value, 46.7550, 1e-4);
	EXPECT_NEAR(r->p_value, 0.03450, 1e-4);
}

}
