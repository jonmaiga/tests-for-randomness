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


namespace mixer {

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

		{test_type::wald_wolfowitz_runs, wald_wolfowitz_test<T>, {}, "ww"}, // weak=xmx
		// {test_type::pearson_r, pearson_correlation_test<T>, {}, "pearson_r"}, // weak
		// {test_type::spearman_r, spearman_correlation_test<T>, {}, "spearman_r"}, // weak
		// {test_type::kendall_tau, kendall_correlation_test<T>, {}, "kendall_tau"}, // slow todo
		{test_type::bit_count_2d, bit_count_2d_test<T>, {}, "bc2d"},
		{test_type::bit_count_3d, bit_count_3d_test<T>, {}, "bc3d"},

		{test_type::gap, gap_test<T>, {}, "gap"}, // weak (found no tweak)
		{test_type::coupon, coupon_test<T>, {}, "coupon"}, // weak (found no tweak)
		{test_type::divisibility, divisibility_test<T>, {}, "divisibility"}, // weak (didn't find any tweak)
		{test_type::permutation, permutation_test<T>, {}, "permutation"}, // weak - but seems tweak-able (might be to little data but w4 also fails 64-bit-xm2x)

		// mixer tests
		{test_type::sac, {}, avalanche_mixer_sac_test<T>, "sac"}, // strong
		{test_type::bic, {}, avalanche_mixer_bic_test<T>, "bic"}, // strong
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
