#include <statistics/kolmogorov.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {

TEST(kolmogorov, large) {
	auto s = test_stream();
	std::vector<double> a;
	while (a.size() < 1000000) {
		a.push_back(rescale_type_to_01(s()));
	}
	const auto stat = kolmogorov_smirnov_stats(a);
	EXPECT_NEAR(stat->value, 0.0007142, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.6873, 1e-4);
}

TEST(kolmogorov, data) {
	const auto pv = {
		0.0907329, 0.932244, 0.114827, 0.568087, 0.776664, 0.952185, 0.104311, 0.039066, 0.0191372, 0.6889, 0.469074, 0.167117, 0.800801, 0.676952, 0.731704, 0.354055, 0.928046, 0.285814, 0.858386, 0.908818, 0.07048, 0.481376, 0.728044, 0.646651, 0.707484, 0.385696, 0.428934, 0.56704, 0.757637, 0.7868, 0.011541,
		0.298083, 0.565069, 0.977096, 0.751907, 0.149083, 0.000146674, 0.1505, 0.423413, 0.786884, 0.064658, 0.624864, 0.727396, 0.244652, 0.491885, 0.189494, 0.365173, 0.596297, 0.937726, 0.53262, 0.361511, 0.24349, 0.654869, 0.830458, 0.738669, 0.78607, 0.549001, 0.563609, 0.034134, 0.643928, 0.748101, 0.866515,
		0.255059, 0.213638, 0.399358, 0.127307, 0.0803425, 0.535384, 0.479835, 0.332602, 0.313224, 0.74566, 0.586695, 0.31661, 0.622685, 0.319371, 0.387137, 0.956242, 0.852309, 0.117861, 0.34173, 0.278867, 0.590599, 0.846015, 0.308734, 0.346514, 0.583689, 0.632231, 0.257446, 0.499904, 0.0834331, 0.262653, 0.1925,
		0.332932, 0.786185, 0.584509, 0.288786, 0.538318, 0.292864, 0.102028
	};
	const auto r = kolmogorov_smirnov_stats(pv);
	EXPECT_NEAR(r->value, 0.0831161, 1e-4); // matches mma
	EXPECT_NEAR(r->p_value, 0.4774783, 1e-4); // almost matches mma (0.469425)
}

TEST(kolmogorov, small_sample_1) {
	const auto pv = {0., 1e-5};
	const auto r = kolmogorov_smirnov_stats(pv);
	EXPECT_NEAR(r->value, 0.9999, 1e-4); // matches mma
	EXPECT_NEAR(r->p_value, 0.0110, 1e-4); // "far" from mma (2e-10)
}

TEST(kolmogorov, small_sample_2) {
	const auto pv = {0., 0.1};
	const auto r = kolmogorov_smirnov_stats(pv);
	EXPECT_NEAR(r->value, 0.9, 1e-4); // matches mma
	EXPECT_NEAR(r->p_value, 0.02970, 1e-4); // "far" from mma (0.02)
}

}
