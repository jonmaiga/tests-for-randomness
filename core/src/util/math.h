#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include "assertion.h"

namespace mixer {namespace constants {
constexpr double EulerGamma = 0.5772156649015328606065120900824024310421593359399235988057672348;
constexpr double Pi = 3.1415926535897932384626433832795028841971693993751058209749445923;
constexpr double PiSqr = Pi * Pi;
}

inline bool is_near(double a, double b, double epsilon = 1e-6) {
	return std::abs(a - b) <= epsilon;
}

inline bool is_valid(double d) {
	return !std::isinf(d) && !std::isnan(d);
}

inline bool is_valid_between_01(double d) {
	return is_valid(d) && d >= 0 && d <= 1;
}

inline double rescale_to_01(double x, double x_min, double x_max) {
	assertion(x_min <= x_max, "min greater than max");
	if (is_near(x_min, x_max)) return .5;
	return (x - x_min) / (x_max - x_min);
}

template <typename T>
double rescale_type_to_01(T x) {
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<T>::max());
	return static_cast<double>(x) / normalizer;
}

inline double log_safe(double x) {
	assertion(is_valid(x), "x is not valid");
	assertion(x >= 0, "negative x to log");
	constexpr double min_p = 1e-16;
	return std::log(std::max(min_p, x));
}

inline double snap_to_01(double x) {
	if (x < 0 && is_near(x, 0)) return 0;
	if (x > 1 && is_near(x, 1)) return 1;
	assertion(x >= 0 && x <= 1, "Unexpected snap value");
	return x;
}

inline uint64_t stirling_second_kind(int n, int k) {
	if (n == 0 && k == 0) {
		return 1;
	}
	if (n == 0 || k == 0) {
		return 0;
	}
	return k * stirling_second_kind(n - 1, k) + stirling_second_kind(n - 1, k - 1);
}

inline double harmonic_asymptotic(double n) {
	return std::log(n) + constants::EulerGamma + 1. / (2. * n);
}

inline double harmonic(uint64_t n) {
	double h = 0;
	for (uint64_t i = 1; i <= n; ++i) {
		h += 1. / static_cast<double>(i);
	}
	return h;
}

inline double to_one_sided(double p) {
	assertion(is_valid_between_01(p), "to_one_sided of invalid p");
	return 1. - 2. * std::abs(p - 0.5);
}

inline double lambert_w_approximation(double x) {
	// from https://en.wikipedia.org/wiki/Lambert_W_function#Asymptotic_expansions
	assertion(x >= 1, "lambert approximation with to low x");
	if (is_near(x, 1)) {
		return 0.5600;
	}
	const auto l1 = log(x);
	const auto l2 = log(l1);
	return l1 - l2 + l2 / l1 + l2 * (l2 - 2) / (2 * l1 * l1);
}

}
