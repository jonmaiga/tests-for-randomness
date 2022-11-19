#include <stream.h>
#include "testutil.h"

#include <gtest/gtest.h>

namespace mixer {

TEST(stream, iterable) {
	const auto s = create_counter_stream(1);
	int j = 0;
	int sum = 0;
	for (auto v : ranged_stream(s, 10)) {
		j++;
		sum += v;
	}
	EXPECT_EQ(j, 10);
	EXPECT_EQ(sum, j*(j-1)/2);
}

TEST(stream, iterator) {
	const auto s = create_counter_stream(1);
	{
		const auto r = ranged_stream(s, 0);
		EXPECT_TRUE(r.begin() == r.end());
	}
	{
		const auto r = ranged_stream(s, 1);
		auto it = r.begin();
		EXPECT_EQ(*it, 0);
		it++;
		EXPECT_EQ(*r.begin(), 0);
		EXPECT_EQ(*it, 1);
		++it;
		EXPECT_EQ(*it, 2);
		EXPECT_TRUE(r.begin() != r.end());
		const stream_iterator n = ++r.begin();
		EXPECT_TRUE(n == r.end());
	}

	{
		const auto r = ranged_stream(s, 10);
		EXPECT_TRUE(r.begin() != r.end());
		EXPECT_TRUE(r.begin() == r.begin());
	}
}


}
