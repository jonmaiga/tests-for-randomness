#pragma once

#include "statistics/avalanche.h"
#include "statistics/basic.h"
#include "statistics/binary_rank.h"
#include "statistics/bit_correlation.h"
#include "statistics/coupon.h"
#include "statistics/divisibility.h"
#include "statistics/gap.h"
#include "statistics/linear_complexity.h"
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

		{test_type::wald_wolfowitz_runs, wald_wolfowitz_test<T>, {}, "runs"},
		{test_type::bit_count_2d, bit_count_2d_test<T>, {}, "bc2d"},

		{test_type::gap, limit_n_slow<T>(gap_test<T>), {}, "gap"},
		{test_type::coupon, limit_n_slower<T>(coupon_test<T>), {}, "coupon"},
		{test_type::divisibility, limit_n_slow<T>(divisibility_test<T>), {}, "divisibility"},
		{test_type::permutation, limit_n_slow<T>(permutation_test<T>), {}, "permutation"},
		{test_type::binary_rank, limit_n_slower<T>(binary_rank_test<T>), {}, "binary-rank"},
		{test_type::linear_complexity, limit_n_slower<T>(linear_complexity_test<T>), {}, "linear-complexity"},

		// mixer tests
		{test_type::sac, {}, limit_n_slow<T>(avalanche_mixer_sac_test<T>), "sac"},
		{test_type::bic, {}, limit_n_slower<T>(avalanche_mixer_bic_test<T>), "bic"},
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
