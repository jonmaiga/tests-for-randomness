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
struct test_definition {
	test_type type{};
	stream_test<T> stream_test;
	mixer_test<T> mixer_test;
	std::string name;
};

template <typename T>
std::vector<test_definition<T>> get_tests() {
	return {
		{test_type::basic_mean, basic_test<T>, {}, "mean"},
		{test_type::chi2, chi2_test<T>, {}, "chi2"},

		// requires more than 1 iteration/sorting
		{test_type::kolmogorov_smirnov, kolmogorov_test<T>, {}, "ks"},
		{test_type::anderson_darling, anderson_darling_test<T>, {}, "ad"},
		{test_type::wald_wolfowitz_runs, wald_wolfowitz_test<T>, {}, "ww"},
		{test_type::pearson_r, pearson_correlation_test<T>, {}, "pearson_r"},
		{test_type::spearman_r, spearman_correlation_test<T>, {}, "spearman_r"},
		{test_type::kendall_tau, kendall_correlation_test<T>, {}, "kendall_tau"},

		{test_type::gap, gap_test<T>, {}, "gap"},
		{test_type::coupon, coupon_test<T>, {}, "coupon"},
		{test_type::divisibility, divisibility_test<T>, {}, "divisibility"},
		{test_type::permutation, permutation_test<T>, {}, "permutation"},

		// mixer tests
		{test_type::sac, {}, avalanche_mixer_sac_test<T>, "sac"},
		{test_type::bic, {}, avalanche_mixer_bic_test<T>, "bic"},
	};
}

template <typename T>
test_definition<T> get_test_definition(test_type type) {
	for (const auto& meta : get_tests<T>()) {
		if (meta.type == type) {
			return meta;
		}
	}
	assertion(false, "could not find meta");
	return {};
}

inline std::string get_test_name(test_type type) {
	return get_test_definition<uint64_t>(type).name;
}

inline std::vector<test_type> all_test_types = []() {
	std::vector<test_type> types;
	for (const auto& meta : get_tests<uint64_t>()) {
		types.push_back(meta.type);
	}
	return types;
}();


}
