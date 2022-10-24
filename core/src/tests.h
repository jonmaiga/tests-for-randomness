#pragma once

#include "streams.h"

namespace mixer {

inline std::vector<test_factory> create_test_streams(const mixer& mixer, uint64_t n) {
	const auto counter1 = [mixer, n]() {
		return test_config{create_counter_stream(1, n), mixer};
	};
	const auto greycode2 = [mixer, n]() {
		return test_config{create_gray_code(n, 2), mixer};
	};
	const auto greycode4 = [mixer, n]() {
		return test_config{create_gray_code(n, 4), mixer};
	};
	const auto greycode8 = [mixer, n]() {
		return test_config{create_gray_code(n, 8), mixer};
	};
	return {counter1, greycode2, greycode4, greycode8};
}

}