#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "util/assertion.h"

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
	mixer mixer;
	append_stream_factory append_stream_factory;
};

using test_factory = std::function<test_config()>;

enum class s_type {
	basic_mean,
	chi2,
	kolmogorov_smirnov,
	wald_wolfowitz_runs,
	anderson_darling,

	// mixer
	sac,
	bic,
	pearson_r,
	spearman_r,
	kendall_tau,

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


}
