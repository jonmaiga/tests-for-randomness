#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "stream.h"
#include "util/assertion.h"
#include "util/math.h"

namespace mixer {


struct mixer {
	std::string name;
	std::function<uint64_t(uint64_t)> mix;

	uint64_t operator()(uint64_t x) const {
		return mix(x);
	}
};

using stream_factory = std::function<stream_uint64()>;
using append_stream_factory = std::function<stream_uint64(const stream_uint64&)>;

struct test_config {
	uint64_t n{};
	stream_uint64 source;
	mixer mix;
	append_stream_factory stream_append_factory;
};

using test_factory = std::function<test_config()>;

enum class statistic_type {
	z_score,
	chi2,
	kolmogorov_smirnov_d,
	anderson_darling_A2,
	pearson_r,
	spearman_r,
	kendall_tau
};

enum class test_type {
	basic_mean,
	chi2,
	kolmogorov_smirnov,
	wald_wolfowitz_runs,
	anderson_darling,
	pearson_r,
	spearman_r,
	kendall_tau,
	gap_low,
	gap_medium,
	gap_high,
	coupon,
	divisibility_2,
	divisibility_3,
	permutation,

	// mixer
	sac,
	bic,

	size
};

struct statistic_meta {
	test_type type;
	std::string name;
};

const auto all_metas = std::vector<statistic_meta>{
	{test_type::basic_mean, "mean"},
	{test_type::chi2, "chi2"},
	{test_type::kolmogorov_smirnov, "kolmogorov-smirnov"},
	{test_type::wald_wolfowitz_runs, "wald-wolfowitz runs"},
	{test_type::anderson_darling, "anderson_darling"},
	{test_type::sac, "sac"},
	{test_type::bic, "bic"},
	{test_type::pearson_r, "pearson r"},
	{test_type::spearman_r, "spearman rho"},
	{test_type::kendall_tau, "kendall tau"}
};

struct statistic {
	statistic_type type;
	double value{};
	double p_value{};
};

struct result {
	std::string stream_name;
	std::string mixer_name;
	test_type type;
	statistic stats;
};

inline statistic_meta get_meta(test_type type) {
	for (const auto& meta : all_metas) {
		if (meta.type == type) {
			return meta;
		}
	}
	assertion(false, "could not find meta");
	return {};
}

using mixer_test = std::function<std::optional<statistic>(uint64_t n, const stream_uint64&, const mixer&)>;
using stream_test = std::function<std::optional<statistic>(uint64_t n, const stream_uint64&)>;

struct test_result {
	using result_map = std::map<test_type, std::vector<result>>;

	std::string name;
	std::string mixer_name;
	result_map results;

	void add(const result& r) {
		assertion(is_valid(r.stats.value), "a statistic is not valid");
		assertion(is_valid_between_01(r.stats.p_value), "a p-value is not valid or normal");
		results[r.type].push_back(r);
	}

	const std::vector<result>& operator[](test_type type) const {
		static const std::vector<result> empty;
		const auto it = results.find(type);
		return it != results.end() ? it->second : empty;
	}
};

using data_fn = std::function<double(std::size_t)>;

template <typename T>
data_fn to_data(const T& d) {
	if constexpr (std::is_arithmetic<T>()) {
		return [d](std::size_t) {
			return static_cast<double>(d);
		};
	}
	else {
		return [&d](std::size_t i) {
			return static_cast<double>(d[i]);
		};
	}
}

inline data_fn mul(const data_fn& a, const data_fn& b) {
	return [&a, &b](std::size_t i) {
		return a(i) * b(i);
	};
}


}
