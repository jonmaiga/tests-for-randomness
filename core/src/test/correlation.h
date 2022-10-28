#pragma once

#include <string>
#include <vector>

namespace mixer {

struct correlation_stats {
	double pearson_r = 0;
};

struct correlation_result {
	std::string stream_name;
	std::string mixer_name;
	uint64_t n = 0;
	correlation_stats stats;
};

inline double pearson_correlation(uint64_t n, const std::vector<double>& xs, const std::vector<double>& ys) {
	double x_mean = 0, y_mean = 0;
	for (uint64_t i = 0; i < n; ++i) {
		x_mean += xs[i];
		y_mean += ys[i];
	}
	x_mean /= n;
	y_mean /= n;

	double sum_xs = 0;
	double sum_ys = 0;
	double sum_xy = 0;
	for (std::size_t i = 0; i < n; ++i) {
		const double x_dev = xs[i] - x_mean;
		const double y_dev = ys[i] - y_mean;
		sum_xs += x_dev * x_dev;
		sum_ys += y_dev * y_dev;
		sum_xy += x_dev * y_dev;
	}
	const double r = sum_xy / (std::sqrt(sum_xs) * std::sqrt(sum_ys));
	return {r};
}


inline correlation_result correlation_mixer_test(uint64_t n, const stream& source, const mixer& mixer) {
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());

	std::vector<double> xs, ys;
	for (uint64_t i = 0; i < n; ++i) {
		const uint64_t v = source();
		const uint64_t m = mixer(v);
		const double x = m / normalizer;
		for (int bit = 0; bit < 64; ++bit) {
			xs.push_back(x);
			ys.push_back(mixer(flip_bit(m, bit)) / normalizer);
		}
	}

	return {
		source.name, mixer.name, n,
		{pearson_correlation(n, xs, ys)}
	};
}

}
