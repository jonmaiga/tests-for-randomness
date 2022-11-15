#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "util/assertion.h"
#include "util/math.h"

namespace mixer {

struct stream {
	std::string name;
	std::function<uint64_t()> next;

	uint64_t operator()() const {
		return next();
	}
};

struct mixer {
	std::string name;
	std::function<uint64_t(uint64_t)> mix;

	uint64_t operator()(uint64_t x) const {
		return mix(x);
	}
};

using stream_factory = std::function<stream()>;
using append_stream_factory = std::function<stream(const stream&)>;

struct test_config {
	uint64_t n{};
	stream source;
	mixer mix;
	append_stream_factory stream_append_factory;
};

using test_factory = std::function<test_config()>;

enum class s_type {
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
	
	// mixer
	sac,
	bic,

	size
};

struct statistic_meta {
	s_type type;
	std::string name;
};

const auto all_metas = std::vector<statistic_meta>{
	{s_type::basic_mean, "mean"},
	{s_type::chi2, "chi2"},
	{s_type::kolmogorov_smirnov, "kolmogorov-smirnov"},
	{s_type::wald_wolfowitz_runs, "wald-wolfowitz runs"},
	{s_type::anderson_darling, "anderson_darling"},
	{s_type::sac, "sac"},
	{s_type::bic, "bic"},
	{s_type::pearson_r, "pearson r"},
	{s_type::spearman_r, "spearman rho"},
	{s_type::kendall_tau, "kendall tau"}
};

struct statistic {
	s_type type;
	double value{};
	double p_value{};
};

struct result {
	std::string stream_name;
	std::string mixer_name;
	statistic stats;
};

inline statistic_meta get_meta(s_type type) {
	for (const auto& meta : all_metas) {
		if (meta.type == type) {
			return meta;
		}
	}
	assertion(false, "could not find meta");
	return {};
}

using mixer_test = const std::function<std::vector<statistic>(uint64_t n, const stream&, const mixer&)>;
using stream_test = const std::function<std::vector<statistic>(uint64_t n, const stream&)>;

struct test_result {
	using result_map = std::map<s_type, std::vector<result>>;

	std::string name;
	std::string mixer_name;
	result_map results;

	void add(const std::vector<result>& rs) {
		for (const auto& r : rs) {
			assertion(is_valid(r.stats.value), "a statistic is not valid");
			assertion(is_valid_between_01(r.stats.p_value), "a p-value is not valid or normal");
			results[r.stats.type].push_back(r);
		}
	}

	const std::vector<result>& operator[](s_type type) const {
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
