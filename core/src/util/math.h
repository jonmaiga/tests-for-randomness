#pragma once

inline double normalize(uint64_t x) {
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());
	return static_cast<double>(x) / normalizer;
}
