#include <util/algo.h>

#include <gtest/gtest.h>

namespace mixer {

inline std::vector<uint64_t> sliding_bit_window(const std::vector<uint64_t>& data,
                                                int window_size) {
	std::vector<uint64_t> r;
	sliding_bit_window(data, window_size, 1, [&r](uint64_t v) { r.push_back(v); });
	return r;
}


TEST(algo, sliding_bit_window) {
	using T = std::vector<uint64_t>;
	EXPECT_EQ(sliding_bit_window({ 0 }, 1).size(), 64);
	EXPECT_EQ(sliding_bit_window({ 0 }, 2).size(), 63);
	EXPECT_EQ(sliding_bit_window({ 0 }, 3).size(), 62);
	EXPECT_EQ(sliding_bit_window({ 0 }, 32).size(), 33);
	EXPECT_EQ(sliding_bit_window({ 1 }, 63).size(), 2);

	EXPECT_EQ(sliding_bit_window({ 2 }, 1)[0], 0);
	EXPECT_EQ(sliding_bit_window({ 2 }, 1)[1], 1);
	EXPECT_EQ(sliding_bit_window({ 1ull << 34 | 3 }, 32)[0], 3);
	EXPECT_EQ(sliding_bit_window({ 1ull << 34 | 3 }, 32)[1], 1);
	EXPECT_EQ(sliding_bit_window({ 1ull << 34 | 3 }, 32)[2], 0);
	EXPECT_EQ(sliding_bit_window({ 1ull << 34 | 3 }, 32)[3], 2147483648ull);
	EXPECT_EQ(sliding_bit_window({ 1 }, 63), (T{ 1,0 }));
	EXPECT_EQ(sliding_bit_window({ 1ull << 63 }, 63), (T{ 0,4611686018427387904ull }));
}

TEST(algo, sliding_bit_window_more) {
	using T = std::vector<uint64_t>;
	EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff }, 1), (T(64, 1)));
	EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff }, 2), (T(63, 3)));
	EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff }, 3), (T(62, 7)));

	{
		auto h = T(64, 1);
		h[0] = 0;
		EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff - 1 }, 1), h);
	}

	{
		auto expected = T(63, 3);
		expected[expected.size() - 1] = 0;
		expected[expected.size() - 2] = 1;
		const auto actual = sliding_bit_window({0xffffffffffffffff >> 2}, 2);
		EXPECT_EQ(actual, expected);
	}
}

TEST(algo, sliding_bit_window_128bits) {
	using T = std::vector<uint64_t>;
	const auto f = 0xffffffffffffffff;
	EXPECT_EQ(sliding_bit_window({ 0, 0 }, 1), T(128,0));
	EXPECT_EQ(sliding_bit_window({ 0, 0 }, 2), T(127,0));
	EXPECT_EQ(sliding_bit_window({ f, f }, 1), T(128,1));
	EXPECT_EQ(sliding_bit_window({ f, f }, 2), T(127,3));
}

TEST(algo, sliding_bit_window_128bits_more) {
	using T = std::vector<uint64_t>;
	const auto f = 0xffffffffffffffff;

	{
		auto expected = T(128, 0);
		std::fill(expected.begin() + 64, expected.end(), 1);
		auto actual = sliding_bit_window({0, f}, 1);
		EXPECT_EQ(actual, expected);
	}

	{
		auto expected = T(127, 0);
		std::fill(expected.begin() + 64, expected.end(), 3);
		expected[63] = 2;
		auto actual = sliding_bit_window({0, f}, 2);
		EXPECT_EQ(actual, expected);
	}

	{
		auto expected = T(126, 0);
		std::fill(expected.begin() + 64, expected.end(), 7);
		expected[62] = 4;
		expected[63] = 6;
		auto actual = sliding_bit_window({0, f}, 3);
		EXPECT_EQ(actual, expected);
	}
}


}
