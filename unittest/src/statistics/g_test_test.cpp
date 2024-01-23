#include <statistics/g_test.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {

TEST(g_test, basic) {
	EXPECT_FALSE(g_test_stats({1}, 1));
	EXPECT_NEAR(g_test_stats({1,1}, 1)->value, 0, 1e-4);
	EXPECT_NEAR(g_test_stats({1,2}, 1)->value, 0.3397, 1e-4);
	EXPECT_NEAR(g_test_stats({80,125,95}, 100)->value, 10.337194, 1e-4);

	using V = std::vector<uint64_t>;
	EXPECT_NEAR(g_test_stats(2, to_data(V{1,2}), to_data(V{1,2}))->value, 0, 1e-4);
	EXPECT_NEAR(g_test_stats(3, to_data(V{80,125,95}), to_data(V{94,112,94}))->value, 3.661414, 1e-4);
}

}