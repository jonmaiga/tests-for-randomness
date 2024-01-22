#include <util/algo.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {

TEST(algo, isolate_bit_0) {
	using T = uint8_t;
	const std::vector<T> d = {1, 0, 1, 1, 0, 0, 0, 1};
	auto s = create_stream_from_data_by_ref("ib", d);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 0), 0b10001101);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 1), 0);
}

TEST(algo, isolate_bit_7) {
	using T = uint8_t;
	const std::vector<T> d = {128, 0, 128, 128, 0, 0, 0, 128};
	auto s = create_stream_from_data_by_ref("ib", d);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 0), 0);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 7), 0b10001101);
}

TEST(algo, isolate_bits) {
	using T = uint8_t;
	const std::vector<T> d = {129, 0, 129, 131, 2, 0, 0, 129};
	auto s = create_stream_from_data_by_ref("ib", d);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 0), 0b10001101);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 7), 0b10001101);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 1), 0b00011000);
}

TEST(algo, isolate_bits_2) {
	using T = uint8_t;
	const std::vector<T> d = {129, 0, 129, 131, 2, 0, 0, 129, 1, 0, 1, 1, 0, 0, 0, 1};
	auto s = create_stream_from_data_by_ref("ib", d);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 7), 0b10001101);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 0), 0b10001101);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 1), 0b00011000);
	EXPECT_EQ(isolate_bit_by_ref<T>(s, 1), 0b00000000);
}

TEST(algo, isolate_bit_by_ref) {
	using T = uint64_t;
	auto s = test_stream();
	auto r1 = isolate_bit_by_ref<T>(s, 0);
	auto r2 = isolate_bit_by_ref<T>(s, 0);
	EXPECT_NE(r1, r2);
}

bool operator==(const merge_bins_result& a, const merge_bins_result& b) {
	return a.expected == b.expected && a.observed == b.observed;
}

TEST(algo, merge_bins) {
	EXPECT_EQ(merge_bins({}, {}, 0), (merge_bins_result{{},{}}));

	EXPECT_EQ(merge_bins({1}, {1}, 0), (merge_bins_result{{1},{1}}));
	EXPECT_EQ(merge_bins({1}, {1}, 1), (merge_bins_result{{1},{1}}));
	EXPECT_EQ(merge_bins({1}, {1}, 2), (merge_bins_result{{},{}}));

	EXPECT_EQ(merge_bins({1,2}, {1,2}, 0), (merge_bins_result{{1,2},{1,2}}));
	EXPECT_EQ(merge_bins({1,2}, {1,2}, 1), (merge_bins_result{{1,2},{1,2}}));
	EXPECT_EQ(merge_bins({1,2}, {1,2}, 2), (merge_bins_result{{3},{3}}));
	EXPECT_EQ(merge_bins({1,2}, {1,2}, 3), (merge_bins_result{{3},{3}}));
	EXPECT_EQ(merge_bins({1,2}, {1,2}, 4), (merge_bins_result{{},{}}));

	// need to go from right as well
	EXPECT_EQ(merge_bins({2,4,8}, {1,2,1}, 2), (merge_bins_result{{14},{4}}));
	EXPECT_EQ(merge_bins({1,2,3,4,5}, {1,1,1,1,1}, 2), (merge_bins_result{{3,12},{2,3}}));
	EXPECT_EQ(merge_bins({1,1,1,1,1,1,1}, {1,1,1,1,1,1,2}, 3), (merge_bins_result{{3,4},{3,5}}));
}

}
