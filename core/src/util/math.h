#pragma once

inline double normalize(uint64_t x) {
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());
	return static_cast<double>(x) / normalizer;
}

inline double half64() {
	constexpr auto half = static_cast<double>(std::numeric_limits<uint64_t>::max() / 2);
	return half;
}
