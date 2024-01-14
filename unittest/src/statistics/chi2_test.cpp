#include <statistics/chi2.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {
template <typename T>
std::optional<statistic> chi2_stats_from_test_stream(uint64_t n, const bin_count_function& bin_counter) {
	auto s = test_stream();
	std::vector<T> a;
	a.reserve(n);
	while (a.size() < n) {
		a.push_back(s());
	}
	return chi2_uniform_stats(a, bin_counter);
}

TEST(chi2, unset) {
	EXPECT_FALSE(chi2_stats({}, 0));
	EXPECT_FALSE(chi2_stats({1}, 0));
}

TEST(chi2, basic) {
	using T = std::vector<double>;

	// same as mma
	EXPECT_NEAR(chi2_uniform_stats(rescale_01_to_type(T{0}), get_bin_count_mathematica)->value, 1, 1e-4);
	EXPECT_NEAR(chi2_uniform_stats(rescale_01_to_type(T{0, 0}), get_bin_count_mathematica)->value, 4, 1e-4);
	EXPECT_EQ(chi2_uniform_stats(rescale_01_to_type(T{0.1}), get_bin_count_mathematica)->value, 1);
	EXPECT_EQ(chi2_uniform_stats(rescale_01_to_type(T{0.1}), get_bin_count_mathematica)->df, 1);
	EXPECT_EQ(chi2_uniform_stats(rescale_01_to_type(T{0.1, 0.1, 0.1, 0.1}), get_bin_count_mathematica)->value, 12);
	EXPECT_EQ(chi2_uniform_stats(rescale_01_to_type(T{0.1, 0.1, 0.1, 0.1}), get_bin_count_mathematica)->df, 3);
	EXPECT_NEAR(chi2_uniform_stats(rescale_01_to_type(T{0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.8}), get_bin_count_mathematica)->value, 3.71428, 1e-4);
	EXPECT_EQ(chi2_uniform_stats(rescale_01_to_type(T{0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.8}), get_bin_count_mathematica)->df, 4);
}

TEST(chi2, large) {
	const auto stat = chi2_stats_from_test_stream<uint64_t>(1000000, get_bin_count_mathematica);
	EXPECT_NEAR(stat->value, 485.4309, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.6941, 1e-4);
}

TEST(chi2, data) {
	constexpr auto pv = {
		0.0907329, 0.932244, 0.114827, 0.568087, 0.776664, 0.952185, 0.104311, 0.039066, 0.0191372, 0.6889, 0.469074, 0.167117, 0.800801, 0.676952, 0.731704, 0.354055, 0.928046, 0.285814, 0.858386, 0.908818, 0.07048, 0.481376, 0.728044, 0.646651, 0.707484, 0.385696, 0.428934, 0.56704, 0.757637, 0.7868, 0.011541,
		0.298083, 0.565069, 0.977096, 0.751907, 0.149083, 0.000146674, 0.1505, 0.423413, 0.786884, 0.064658, 0.624864, 0.727396, 0.244652, 0.491885, 0.189494, 0.365173, 0.596297, 0.937726, 0.53262, 0.361511, 0.24349, 0.654869, 0.830458, 0.738669, 0.78607, 0.549001, 0.563609, 0.034134, 0.643928, 0.748101, 0.866515,
		0.255059, 0.213638, 0.399358, 0.127307, 0.0803425, 0.535384, 0.479835, 0.332602, 0.313224, 0.74566, 0.586695, 0.31661, 0.622685, 0.319371, 0.387137, 0.956242, 0.852309, 0.117861, 0.34173, 0.278867, 0.590599, 0.846015, 0.308734, 0.346514, 0.583689, 0.632231, 0.257446, 0.499904, 0.0834331, 0.262653, 0.1925,
		0.332932, 0.786185, 0.584509, 0.288786, 0.538318, 0.292864, 0.102028
	};
	const auto s = chi2_uniform_stats(rescale_01_to_type(pv), get_bin_count_mathematica);
	EXPECT_NEAR(s->value, 8.68, 1e-4); // matches mma
	EXPECT_NEAR(s->p_value, 0.729992, 1e-4); // matches mma
}

TEST(chi2, inconclusive) {
	const std::vector<uint64_t> histogram{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const std::vector<double> expected_probabilities{.1, .1, .1, .1, .1, .1, .1, .1, .1, .1};
	const auto total_count = accumulate(histogram);
	const auto s = chi2_stats(histogram.size(), to_data(histogram), mul(to_data(expected_probabilities), to_data(total_count)), 1.);
	EXPECT_FALSE(s);
}

TEST(chi2, get_bin_count_mathematica) {
	EXPECT_EQ(get_bin_count_mathematica(1), 2);
	EXPECT_EQ(get_bin_count_mathematica(2), 3);
	EXPECT_EQ(get_bin_count_mathematica(3), 4);
	EXPECT_EQ(get_bin_count_mathematica(10), 6);
	EXPECT_EQ(get_bin_count_mathematica(100), 13);
	EXPECT_EQ(get_bin_count_mathematica(1000), 32);
	EXPECT_EQ(get_bin_count_mathematica(1000000), 503);
	EXPECT_EQ(get_bin_count_mathematica(67108864), 2703);
}

TEST(chi2, get_bin_count_pow2_8) {
	auto bc = create_bin_count_pow2<uint8_t>();
	EXPECT_EQ(bc(1), 2);
	EXPECT_EQ(bc(2), 4);
	EXPECT_EQ(bc(3), 4);
	EXPECT_EQ(bc(10), 8);
	EXPECT_EQ(bc(100), 16);
	EXPECT_EQ(bc(1000), 32);
	EXPECT_EQ(bc(1000000), 256);
	EXPECT_EQ(bc(67108864), 256);
}

TEST(chi2, get_bin_count_pow2_16) {
	auto bc = create_bin_count_pow2<uint16_t>();
	EXPECT_EQ(bc(1), 2);
	EXPECT_EQ(bc(2), 4);
	EXPECT_EQ(bc(3), 4);
	EXPECT_EQ(bc(10), 8);
	EXPECT_EQ(bc(100), 16);
	EXPECT_EQ(bc(1000), 32);
	EXPECT_EQ(bc(1000000), 512);
	EXPECT_EQ(bc(67108864), 4096);
	EXPECT_EQ(bc(1ull<<34), 32768);
	EXPECT_EQ(bc(1ull<<35), 65536);
	EXPECT_EQ(bc(1ull<<36), 65536);
	EXPECT_EQ(bc(1ull<<40), 65536);
}

TEST(chi2, get_bin_count_pow2_32) {
	auto bc = create_bin_count_pow2<uint32_t>();
	EXPECT_EQ(bc(1), 2);
	EXPECT_EQ(bc(2), 4);
	EXPECT_EQ(bc(3), 4);
	EXPECT_EQ(bc(10), 8);
	EXPECT_EQ(bc(100), 16);
	EXPECT_EQ(bc(1ull<<40), 262144);
	EXPECT_EQ(bc(1ull<<50), 4194304);
	EXPECT_EQ(bc(1ull<<63), 134217728);
}

TEST(chi2, uniform_8) {
	using T = uint8_t;
	const auto stat = chi2_stats_from_test_stream<T>(1 << 22, create_bin_count_pow2<T>());
	EXPECT_NEAR(stat->value, 254.0640, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.5047, 1e-4);
}

TEST(chi2, uniform_16) {
	using T = uint16_t;
	const auto stat = chi2_stats_from_test_stream<T>(1 << 22, create_bin_count_pow2<T>());
	EXPECT_NEAR(stat->value, 1020.8432, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.5131, 1e-4);
}

TEST(chi2, uniform_32) {
	using T = uint32_t;
	const auto stat = chi2_stats_from_test_stream<T>(1 << 22, create_bin_count_pow2<T>());
	EXPECT_NEAR(stat->value, 985.0458, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.7981, 1e-4);
}

TEST(chi2, uniform_64) {
	using T = uint64_t;
	const auto stat = chi2_stats_from_test_stream<T>(1 << 22, create_bin_count_pow2<T>());
	EXPECT_NEAR(stat->value, 1036.2822, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.3793, 1e-4);
}
}
