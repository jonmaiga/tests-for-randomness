#include <stream.h>
#include "testutil.h"

#include <gtest/gtest.h>

namespace tfr {


TEST(stream, expected_behavior) {
	auto s1 = create_counter_stream<uint64_t>(1);
	EXPECT_EQ(s1(), 1);
	EXPECT_EQ(s1(), 2);

	auto s2 = s1;
	EXPECT_EQ(s2(), 3);
	EXPECT_EQ(s2(), 4);
	EXPECT_EQ(s1(), 3);
	EXPECT_EQ(s1(), 4);

	auto s3 = create_counter_stream<uint64_t>(1);
	EXPECT_EQ(s1(), 5);
	EXPECT_EQ(s2(), 5);
	EXPECT_EQ(s3(), 1);
	EXPECT_EQ(s3(), 2);

	auto s4 = create_counter_stream<uint64_t>(1);
	auto rs = rescale_type_to_01(s4);
	auto d = rs();
	EXPECT_TRUE(is_near(d, 0));
	EXPECT_EQ(s4(), 1);
	EXPECT_EQ(s4(), 2);
}

TEST(stream, iterable) {
	const auto s = create_counter_stream<uint64_t>(1);
	int j = 0;
	int sum = 0;
	for (auto v : ranged_stream(s, 10)) {
		j++;
		sum += v;
	}
	EXPECT_EQ(j, 10);
	EXPECT_EQ(sum, j*(j+1)/2);

	j = 0;
	sum = 0;
	for (auto v : ranged_stream(s, 10)) {
		j++;
		sum += v;
	}
	EXPECT_EQ(j, 10);
	EXPECT_EQ(sum, j*(j+1)/2);
}

TEST(stream, iterate_twice_on_same_ranged) {
	const auto s = create_counter_stream<uint64_t>(1);
	int j = 0;
	int sum = 0;
	auto r = ranged_stream(s, 10);
	for (auto v : r) {
		j++;
		sum += v;
	}
	EXPECT_EQ(j, 10);
	EXPECT_EQ(sum, j*(j+1)/2);

	j = 0;
	sum = 0;
	for (auto v : r) {
		j++;
		sum += v;
	}
	EXPECT_EQ(j, 10);
	EXPECT_EQ(sum, j*(j+1)/2);
}

TEST(stream, iterator) {
	const auto s = create_counter_stream<uint64_t>(1);
	{
		const auto r = ranged_stream(s, 0);
		EXPECT_TRUE(r.begin() == r.end());
	}
	{
		const auto r = ranged_stream(s, 1);
		EXPECT_TRUE(r.begin() != r.end());

		auto it = r.begin();
		EXPECT_EQ(*it, 1);
		++it;
		EXPECT_EQ(it, r.end());
		EXPECT_EQ(*r.begin(), 1);
		
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
