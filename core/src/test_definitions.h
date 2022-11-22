#pragma once

#include "statistics/andersondarling.h"
#include "statistics/avalanche.h"
#include "statistics/basic.h"
#include "statistics/chi2.h"
#include "statistics/correlation.h"
#include "statistics/coupon.h"
#include "statistics/divisibility.h"
#include "statistics/gap.h"
#include "statistics/kolmogorov.h"
#include "statistics/permutation.h"
#include "statistics/waldwolfowitz.h"


namespace mixer {

template <typename T>
struct stream_test_definition {
	test_type type;
	stream_test<T> test;
};

template <typename T>
std::vector<stream_test_definition<T>> get_stream_tests() {
	return {
		{test_type::basic_mean, basic_test<T>},
		{test_type::chi2, chi2_test<T>},

		// requires more than 1 iteration/sorting
		{test_type::kolmogorov_smirnov, kolmogorov_test<T>},
		{test_type::anderson_darling, anderson_darling_test<T>},
		{test_type::wald_wolfowitz_runs, wald_wolfowitz_test<T>},
		{test_type::pearson_r, pearson_correlation_test<T>},
		{test_type::spearman_r, spearman_correlation_test<T>},
		{test_type::kendall_tau, kendall_correlation_test<T>},

		{test_type::gap, gap_test<T>},
		{test_type::coupon, coupon_test<T>},
		{test_type::divisibility, divisibility_test<T>},
		{test_type::permutation, permutation_test<T>}
	};
}

template <typename T>
struct mixer_test_definition {
	test_type type;
	mixer_test<T> test;
};

template <typename T>
std::vector<mixer_test_definition<T>> get_mixer_tests() {
	return {
		{test_type::sac, avalanche_mixer_sac_test<T>},
		{test_type::bic, avalanche_mixer_bic_test<T>},
	};
}

}
