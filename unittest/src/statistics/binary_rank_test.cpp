#include <statistics/binary_rank.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {
binary_square_matrix create_binary_square_matrix(const std::vector<std::vector<uint8_t>>& m) {
	auto bm = binary_square_matrix(m.size());
	assertion(m.empty() || m.size() == m.front().size(), "Not a square matrix");
	for (std::size_t r = 0; r < m.size(); ++r) {
		for (std::size_t c = 0; c < m.size(); ++c) {
			const auto v = m[r][c];
			assertion(v == 0 || v == 1, "Not a binary matrix");
			bm.set(r, c, v);
		}
	}
	return bm;
}

uint64_t calculate_rank(const std::vector<std::vector<uint8_t>>& m) {
	auto bm = create_binary_square_matrix(m);
	return row_reduce_and_rank(bm);
}

TEST(binary_rank, calculate_rank_small) {
	EXPECT_EQ(calculate_rank({}), 0);
	EXPECT_EQ(calculate_rank({{0}}), 0);
	EXPECT_EQ(calculate_rank({{1}}), 1);

	EXPECT_EQ(calculate_rank({{0,0},{0,0}}), 0);

	EXPECT_EQ(calculate_rank({{1,0},{0,0}}), 1);
	EXPECT_EQ(calculate_rank({{0,1},{0,0}}), 1);
	EXPECT_EQ(calculate_rank({{0,0},{1,0}}), 1);
	EXPECT_EQ(calculate_rank({{0,0},{0,1}}), 1);

	EXPECT_EQ(calculate_rank({{1,1},{0,0}}), 1);
	EXPECT_EQ(calculate_rank({{1,0},{1,0}}), 1);
	EXPECT_EQ(calculate_rank({{1,0},{0,1}}), 2);
	EXPECT_EQ(calculate_rank({{0,1},{1,0}}), 2);
	EXPECT_EQ(calculate_rank({{0,1},{0,1}}), 1);
	EXPECT_EQ(calculate_rank({{0,0},{1,1}}), 1);

	EXPECT_EQ(calculate_rank({{1,1},{1,0}}), 2);
	EXPECT_EQ(calculate_rank({{1,0},{1,1}}), 2);
	EXPECT_EQ(calculate_rank({{0,1},{1,1}}), 2);

	EXPECT_EQ(calculate_rank({{1,1},{1,1}}), 1);
}

TEST(binary_rank, calculate_rank_small_3_3) {
	EXPECT_EQ(calculate_rank({{0,0,0},{0,0,0},{0,0,0}}), 0);
	EXPECT_EQ(calculate_rank({{0,0,0},{0,0,0},{0,0,1}}), 1);
	EXPECT_EQ(calculate_rank({{1,0,0},{0,1,0},{0,0,1}}), 3);
	EXPECT_EQ(calculate_rank({{1,1,1},{0,1,0},{1,0,1}}), 2);
	EXPECT_EQ(calculate_rank({{1,0,1},{1,1,0},{0,0,1}}), 3);
}

std::vector<binary_square_matrix> get_matrices(int matrix_size, uint64_t bits_low, uint64_t bits_high) {
	using V = std::vector<uint64_t>;
	std::vector<binary_square_matrix> ms;
	for_each_matrix(V{bits_low, bits_high}, matrix_size, [&ms](const binary_square_matrix& m) {
		ms.push_back(m);
	});
	return ms;
}

TEST(binary_rank, for_each_matrix_2_2) {
	const auto ms = get_matrices(2, 0b1010111100000100, 0b0011);
	EXPECT_EQ(ms.size(), 32);
	EXPECT_EQ(create_binary_square_matrix({{0,0},{1,0}}), ms[0]);
	EXPECT_EQ(create_binary_square_matrix({{0,0},{0,0}}), ms[1]);
	EXPECT_EQ(create_binary_square_matrix({{1,1},{1,1}}), ms[2]);
	EXPECT_EQ(create_binary_square_matrix({{0,1},{0,1}}), ms[3]);
	EXPECT_EQ(create_binary_square_matrix({{1,1},{0,0}}), ms[16]);
}

TEST(binary_rank, for_each_matrix_3_3) {
	constexpr uint64_t low = 0b111010111100000100 | static_cast<uint64_t>(0b1101101111) << 54;
	const auto ms = get_matrices(3, low, 0b10000000);
	EXPECT_EQ(ms.size(), 14);
	EXPECT_EQ(create_binary_square_matrix({{0,0,1},{0,0,0}, {0,0,1}}), ms[0]);
	EXPECT_EQ(create_binary_square_matrix({{1,1,1},{0,1,0}, {1,1,1}}), ms[1]);
	EXPECT_EQ(create_binary_square_matrix({{1,1,1},{1,0,1}, {1,0,1}}), ms[6]);
	EXPECT_EQ(create_binary_square_matrix({{1,0,0},{0,0,0}, {0,0,1}}), ms[7]);
}

TEST(binary_rank, no_change) {
	using T = uint64_t;
	const auto stat = binary_rank_stats<T>(1 << 10, test_stream(), 16);
	EXPECT_NEAR(stat->value, 2.7871, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.24822, 1e-4);
}

TEST(binary_rank, no_change_bit_isolation) {
	using T = uint64_t;
	const auto stat = binary_rank_isolated_bit_stats<T>(1 << 15, test_stream(), 32, 0);
	EXPECT_NEAR(stat->value, 5.1677, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.1599, 1e-4);
}

TEST(binary_rank, large) {
	using T = uint64_t;
	const auto stat = binary_rank_stats<T>(1 << 15, test_stream(), 32);
	EXPECT_NEAR(stat->value, 0.9397, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.8158, 1e-4);
}

TEST(binary_rank, rank_probability) {
	EXPECT_NEAR(get_rank_probability(4,4), 0.3076171875, 1e-8);
	EXPECT_NEAR(get_rank_probability(4,3), 0.5767822265, 1e-8);
	EXPECT_NEAR(get_rank_probability(4,2), 0.1121520996, 1e-8);
	EXPECT_NEAR(get_rank_probability(4,1), 0.0034332275, 1e-8);
	EXPECT_NEAR(get_rank_probability(4,0), 0.0000152587, 1e-8);

	EXPECT_NEAR(get_rank_probability(5,5), 0.2980041503, 1e-8);
	EXPECT_NEAR(get_rank_probability(5,3), 0.1202881336, 1e-8);
	EXPECT_NEAR(get_rank_probability(5,0), 2.9802322387e-8, 1e-8);

	EXPECT_NEAR(get_rank_probability(64,64), 0.2887880950, 1e-8);
	EXPECT_NEAR(get_rank_probability(64,63), 0.5775761901, 1e-8);
	EXPECT_NEAR(get_rank_probability(64,62), 0.1283502644, 1e-8);
	EXPECT_NEAR(get_rank_probability(64,0), 0, 1e-8);

	EXPECT_NEAR(get_rank_probability(1024,1024), 0.2887880950, 1e-8);
	EXPECT_NEAR(get_rank_probability(1024,1023), 0.5775761901, 1e-8);
	EXPECT_NEAR(get_rank_probability(1024,1022), 0.1283502644, 1e-8);
	EXPECT_NEAR(get_rank_probability(1024,1021), 0.0052387863, 1e-8);
	EXPECT_NEAR(get_rank_probability(1024,1020), 0.0000465669, 1e-8);
}

TEST(binary_rank, rank_probabilities_small) {
	const auto ps = get_rank_probabilities(4);
	EXPECT_EQ(ps.size(), 5);
	EXPECT_NEAR(ps[0], 0.3076, 1e-4);
	EXPECT_NEAR(ps[1], 0.5767, 1e-4);
}

TEST(binary_rank, rank_probabilities_large) {
	const auto ps = get_rank_probabilities(1000);
	EXPECT_EQ(ps.size(), 1001);
	EXPECT_NEAR(ps[0], 0.2887880950, 1e-4);
	EXPECT_NEAR(ps[1], 0.5775761901, 1e-4);
}

TEST(binary_rank, get_matrix_size) {
	using T = uint64_t;
	EXPECT_EQ(get_matrix_size<T>(1<<10), 8);
	EXPECT_EQ(get_matrix_size<T>(1<<11), 8);
	EXPECT_EQ(get_matrix_size<T>(1<<12), 16);
	EXPECT_EQ(get_matrix_size<T>(1<<13), 16);
	EXPECT_EQ(get_matrix_size<T>(1<<14), 32);
	EXPECT_EQ(get_matrix_size<T>(1<<15), 32);
	EXPECT_EQ(get_matrix_size<T>(1<<16), 64);
	EXPECT_EQ(get_matrix_size<T>(1<<17), 64);
	EXPECT_EQ(get_matrix_size<T>(1<<18), 128);
	EXPECT_EQ(get_matrix_size<T>(1<<19), 128);
	EXPECT_EQ(get_matrix_size<T>(1<<20), 256);
	EXPECT_EQ(get_matrix_size<T>(1ull<<30), 8192);
	EXPECT_EQ(get_matrix_size<T>(1ull<<31), 8192);
	EXPECT_EQ(get_matrix_size<T>(1ull<<32), 16384);
	EXPECT_EQ(get_matrix_size<T>(1ull<<39), 131072);
}

TEST(binary_rank, get_matrix_size_32) {
	using T = uint32_t;
	EXPECT_EQ(get_matrix_size<T>(1<<10), 4);
	EXPECT_EQ(get_matrix_size<T>(1<<11), 8);
	EXPECT_EQ(get_matrix_size<T>(1<<12), 8);
	EXPECT_EQ(get_matrix_size<T>(1<<13), 16);
	EXPECT_EQ(get_matrix_size<T>(1<<14), 16);
	EXPECT_EQ(get_matrix_size<T>(1ull<<30), 4096);
	EXPECT_EQ(get_matrix_size<T>(1ull<<31), 8192);
	EXPECT_EQ(get_matrix_size<T>(1ull<<32), 8192);
	EXPECT_EQ(get_matrix_size<T>(1ull<<33), 16384);
}
}
