#pragma once

#include "streams.h"

namespace mixer {

inline std::vector<test_factory> create_test_streams(const mixer& mixer, uint64_t n) {
	const auto counter1 = [mixer, n]() {
		return test_config{n, create_counter_stream(1), mixer};
	};
	const auto greycode2 = [mixer, n]() {
		return test_config{n, create_gray_code(2), mixer};
	};
	const auto greycode4 = [mixer, n]() {
		return test_config{n, create_gray_code(4), mixer};
	};
	const auto greycode8 = [mixer, n]() {
		return test_config{n, create_gray_code(8), mixer};
	};
	return {counter1, greycode2, greycode4, greycode8};
}

}