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

struct stream_test_definition {
	test_type type;
	stream_test test;
};

const std::vector<stream_test_definition> stream_tests = {
	{test_type::basic_mean, basic_test},
	{test_type::chi2, chi2_test},
	{test_type::kolmogorov_smirnov, kolmogorov_test},
	{test_type::anderson_darling, anderson_darling_test},
	{test_type::wald_wolfowitz_runs, wald_wolfowitz_test},
	{test_type::pearson_r, pearson_correlation_test},
	{test_type::spearman_r, spearman_correlation_test},
	{test_type::kendall_tau, kendall_correlation_test},
	{test_type::gap_low, create_gap_test(0, 0.33)},
	{test_type::gap_medium, create_gap_test(0.33, 0.66)},
	{test_type::gap_high, create_gap_test(0.66, 1)},
	{test_type::coupon, coupon_test},
	{test_type::divisibility_2, create_divisibility_test(2)},
	{test_type::divisibility_2, create_divisibility_test(3)},
	{test_type::permutation, permutation_test}
};

struct mixer_test_definition {
	test_type type;
	mixer_test test;
};

const std::vector<mixer_test_definition> mixer_tests = {
	{test_type::sac, avalanche_mixer_sac_test},
	{test_type::bic, avalanche_mixer_bic_test},
};

}
