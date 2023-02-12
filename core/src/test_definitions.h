#pragma once

#include "statistics/avalanche.h"
#include "statistics/basic.h"
#include "statistics/bit_correlation.h"
#include "statistics/correlation.h"
#include "statistics/coupon.h"
#include "statistics/divisibility.h"
#include "statistics/gap.h"
#include "statistics/permutation.h"
#include "statistics/uniform.h"
#include "statistics/waldwolfowitz.h"


namespace tfr {

template <typename T>
struct test_definition {
	test_type type{};
	stream_test<T> test_stream;
	mixer_test<T> test_mixer;
	std::string name;
};

template <typename T>
std::vector<test_definition<T>> get_tests() {
	return {
		{test_type::mean, mean_test<T>, {}, "mean"},
		{test_type::uniform, uniform_test<T>, {}, "uniform"},

		{test_type::wald_wolfowitz_runs, wald_wolfowitz_test<T>, {}, "ww"},
		// {test_type::pearson_r, pearson_correlation_test<T>, {}, "pearson_r"},
		// {test_type::spearman_r, spearman_correlation_test<T>, {}, "spearman_r"},
		// {test_type::kendall_tau, kendall_correlation_test<T>, {}, "kendall_tau"},
		{test_type::bit_count_2d, bit_count_2d_test<T>, {}, "bc2d"},
		{test_type::bit_count_3d, bit_count_3d_test<T>, {}, "bc3d"},

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
	for (const auto& test_def : get_tests<T>()) {
		if (test_def.type == type) {
			return test_def;
		}
	}
	assertion(false, "could not find test definition");
	return {};
}

template <typename T>
std::vector<test_definition<T>> get_most_effective_tests() {
	return {
		get_test_definition<T>(test_type::uniform),
		get_test_definition<T>(test_type::divisibility), // stops 32/32/28/1049346062286479453 at 17 instead of 25
		get_test_definition<T>(test_type::bit_count_2d),
		get_test_definition<T>(test_type::sac),

	};
}

inline std::string get_test_name(test_type type) {
	return get_test_definition<uint64_t>(type).name;
}

inline std::vector<test_type> default_test_types = []() {
	std::vector<test_type> types;
	for (const auto& test_def : get_tests<uint64_t>()) {
		types.push_back(test_def.type);
	}
	return types;
}();


}
