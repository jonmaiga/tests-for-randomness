#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

#include "assertion.h"

inline bool is_near(double a, double b, double epsilon = 1e-6) {
	return std::abs(a - b) <= epsilon;
}

inline bool is_valid(double d) {
	return !std::isinf(d) && !std::isnan(d);
}

inline bool is_valid_normal(double d) {
	return !std::isinf(d) && !std::isnan(d) && d >= 0 && d <= 1;
}

inline double normalize(double x, double x_min, double x_max, double epsilon = 1e-6) {
	assertion(x_min <= x_max, "min greater than max");
	if (is_near(x_min, x_max)) return .5;
	double s = (x - x_min) / (x_max - x_min);
	s *= (1 - 2 * epsilon);
	s += epsilon;
	return s;
}

inline double normalize64(uint64_t x) {
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());
	return normalize(static_cast<double>(x) / normalizer, 0, 1);
}
