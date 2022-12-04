#pragma once

#include "chi2.h"

namespace mixer {

template <typename T>
sub_test_results uniform_test(const uint64_t n, const stream<T>& source) {
	//return split_test(n, 1000000, [&source](uint64_t size) {
	//	return anderson_darling_stats(rescale_type_to_01_by_ref(size, source));
	//});

	//return split_test(n, 1000000, [&source](uint64_t size) {
	//	return kolmogorov_smirnov_stats(rescale_type_to_01_by_ref(size, source));
	//});

	return main_sub_test(chi2_uniform_stats(ranged_stream(rescale_type_to_01(source), n)));
}

}
