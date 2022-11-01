#pragma once

#include <cstdint>
#include <limits>
#include <utility>

inline double normalize64(uint64_t x) {
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());
	return static_cast<double>(x) / normalizer;
}

inline double half64() {
	constexpr auto half = static_cast<double>(std::numeric_limits<uint64_t>::max() / 2);
	return half;
}

inline double clamp_exclusive_01(double x) {
	constexpr double epsilon = 1e-9;
	return std::max(epsilon, std::min(x, 1. - epsilon));
}
