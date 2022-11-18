#include <statistics/kolmogorov.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(kolmogorov, no_change) {
	const auto r = kolmogorov_test(50, test_stream());
	EXPECT_NEAR(r->value, 0.0971, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7113, 1e-4);
}

}
