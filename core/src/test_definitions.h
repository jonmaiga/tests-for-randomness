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

const std::vector<stream_test> stream_tests = {
	basic_test,
	chi2_test,
	kolmogorov_test,
	anderson_darling_test,
	wald_wolfowitz_test,
	pearson_correlation_test,
	spearman_correlation_test,
	kendall_correlation_test,
	create_gap_test(test_type::gap_low, 0, 0.33),
	create_gap_test(test_type::gap_medium, 0.33, 0.66),
	create_gap_test(test_type::gap_high, 0.66, 1),
	coupon_test,
	divisibility_test,
	permutation_test
};

const std::vector<mixer_test> mixer_tests = {
	avalanche_mixer_sac_test,
	avalanche_mixer_bic_test,
};

}
