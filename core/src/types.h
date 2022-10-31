#pragma once

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
	basic_variance,
	chi2,
	kolmogorov_smirnov,
	wald_wolfowitz_runs,
	anderson_darling,

	// mixer
	avalanche_max_bias,
	pearson_r,
	spearman_r,
	kendall_tau,
};

struct statistic {
	s_type type;
	double value{};
	std::optional<double> p_value;
};

struct result {
	std::string stream_name;
	std::string mixer_name;
	statistic stats;
};


}
