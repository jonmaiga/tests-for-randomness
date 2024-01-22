#include <statistics/permutation.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace tfr {
template <typename T>
std::vector<uint64_t> get_histogram(const std::vector<T>& d) {
	auto s = create_stream_from_data("d", d, 0);
	return get_permutation_histogram(ranged_stream(s, d.size()), 5);
}

TEST(permutation, no_change) {
	const auto r = permutation_test(1000, test_stream());
	EXPECT_NEAR(r.front().stats->value, 116.2279, 1e-4);
	EXPECT_NEAR(r.front().stats->p_value, 0.7434, 1e-4);
}

TEST(permutation, no_change_large) {
	const auto r = permutation_test(1ull << 18, create_stream_from_mixer(create_counter_stream<uint32_t>(1), mix32::mx3));
	EXPECT_NEAR(r.front().stats->value, 16398.3950, 1e-4);
	EXPECT_NEAR(r.front().stats->p_value, 0.4646, 1e-4);
}

TEST(permutation, histogram_zeroes) {
	using T = uint32_t;

	auto h = get_histogram<T>({0});
	EXPECT_EQ(h[0], 6);
	EXPECT_EQ(accumulate(h), 6);

	h = get_histogram<T>({0, 0});
	EXPECT_EQ(h[0], 12);
	EXPECT_EQ(accumulate(h), 12);

	h = get_histogram<T>({0, 0, 0});
	EXPECT_EQ(h[0], 19);
	EXPECT_EQ(accumulate(h), 19);
}

TEST(permutation, histogram) {
	using T = uint32_t;

	auto h = get_histogram<T>({
		0b10001010010000011000100000100000, // 0-5  (2 left)
		0b11000101101010010010100000111001, // 6-11 (4 left)
		0b01001010001100000111101110011010, // 12-18 (1 left)
	});
	for (int i = 0; i < 19; i++) {
		EXPECT_EQ(h[i], 1);
	}
	EXPECT_EQ(accumulate(h), 19);
}

TEST(permutation, no_change_8) {
	auto n = 1 << 20;
	const auto r = permutation_test(n, test_stream_casted<uint8_t>(n)).front().stats;
	EXPECT_NEAR(r->value, 16281.0356, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7125, 1e-4);
}

TEST(permutation, get_permutation_size) {
	using T = uint64_t;
	EXPECT_EQ(get_permutation_size<T>(1<<10), 8);
	EXPECT_EQ(get_permutation_size<T>(1<<15), 12);
	EXPECT_EQ(get_permutation_size<T>(1<<20), 16);
	EXPECT_EQ(get_permutation_size<T>(1<<25), 21);
	EXPECT_EQ(get_permutation_size<T>(1<<30), 26);
	EXPECT_EQ(get_permutation_size<T>(1ull<<35), 31);
}

TEST(permutation, get_permutation_size_32) {
	using T = uint32_t;
	EXPECT_EQ(get_permutation_size<T>(1<<10), 7);
	EXPECT_EQ(get_permutation_size<T>(1<<15), 11);
	EXPECT_EQ(get_permutation_size<T>(1<<20), 16);
	EXPECT_EQ(get_permutation_size<T>(1<<24), 19);
	EXPECT_EQ(get_permutation_size<T>(1<<25), 20);
	EXPECT_EQ(get_permutation_size<T>(1<<26), 21);
	EXPECT_EQ(get_permutation_size<T>(1<<27), 22);
	EXPECT_EQ(get_permutation_size<T>(1<<30), 25);
	EXPECT_EQ(get_permutation_size<T>(1ull<<35), 30);
}
}
