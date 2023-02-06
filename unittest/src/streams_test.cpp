#include <streams.h>
#include "testutil.h"

#include <gtest/gtest.h>

namespace tfr {

TEST(streams, rescale_64_to_01) {
	const auto s = rescale_type_to_01(test_stream());
	int j = 0;
	double sum = 0;
	for (const auto v : ranged_stream(s, 10)) {
		j++;
		sum += v;
	}

	j = 0;
	double sum2 = 0;
	for (const auto v : ranged_stream(s, 10)) {
		j++;
		sum2 += v;
	}
	EXPECT_EQ(j, 10);
	EXPECT_EQ(sum, sum2);
	EXPECT_NEAR(sum, 4.9435, 1e-4);
}

}
