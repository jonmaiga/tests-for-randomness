#include <statistics/permutation.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace mixer {

TEST(permutation, no_change) {
	const auto r = permutation_test(1000, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 46.7550, 1e-4);
	EXPECT_NEAR(r->p_value, 0.03450, 1e-4);
}

//TEST(permutation, permutation_fail_irl) {
//	const auto r = permutation_test(1ull << 21, create_trng_stream<uint32_t>(602142720)).front().stats; //64-bit: 301071360
//	EXPECT_GT(r->p_value, 1e-4, 1e-4);
//}

template <typename T>
std::vector<uint64_t> get_histogram(const std::vector<T>& d) {
	auto s = create_stream_from_data_by_ref<T>("d", d, 0);
	return get_permutation_histogram(ranged_stream(s, d.size()), 5);
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

}
