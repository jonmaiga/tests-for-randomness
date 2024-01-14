#pragma once

#include "chi2.h"

namespace tfr {
template <typename T>
sub_test_results uniform_test(const uint64_t n, const stream<T>& source) {
	return main_sub_test(
		chi2_uniform_stats(ranged_stream(source, n), create_bin_count_pow2<T>()
		)
	);
}
}
