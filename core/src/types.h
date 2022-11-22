#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "mixer.h"
#include "stream.h"
#include "util/assertion.h"
#include "util/math.h"

namespace mixer {

template <typename T>
using stream_factory = std::function<stream<T>()>;

template <typename T>
using append_stream_factory = std::function<stream<T>(const stream<T>&)>;

template <typename T>
struct test_config {
	uint64_t n{};
	stream<T> source;
	mixer<T> mix;
	append_stream_factory<T> stream_append_factory;
};

template <typename T>
using test_factory = std::function<test_config<T>()>;


///////////////////////////////////////////////////////////////
/// STATISTIC TYPES
///////////////////////////////////////////////////////////////

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

	// requires more than 1 iteration
	kolmogorov_smirnov,
	wald_wolfowitz_runs,
	anderson_darling,
	pearson_r,
	spearman_r,
	kendall_tau,

	gap,
	coupon,
	divisibility,
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
	{test_type::pearson_r, "pearson r"},
	{test_type::spearman_r, "spearman rho"},
	{test_type::kendall_tau, "kendall tau"},
	{test_type::gap, "gap"},
	{test_type::coupon, "coupon"},
	{test_type::divisibility, "divisibility"},
	{test_type::permutation, "permutation"},
	{test_type::sac, "sac"},
	{test_type::bic, "bic"},
};

struct statistic {
	statistic(statistic_type type, double value, double p_value, double df) :
		type(type), value(value), p_value(p_value), df(df) {
		assertion(is_valid(value), "statistic value not valid");
		assertion(is_valid_between_01(p_value), "statistic p-value not valid");
		assertion(df > 0, "statistic df not valid");
	}

	statistic_type type;
	double value{};
	double p_value{};
	double df{};
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

///////////////////////////////////////////////////////////////
/// RESULT TYPES
///////////////////////////////////////////////////////////////

struct sub_test {
	std::string name;
	std::optional<statistic> stats;
};

using sub_test_results = std::vector<sub_test>;

inline sub_test_results main_sub_test(std::optional<statistic> s) {
	return {{"m", s}};
}

struct test_key {
	test_type type;
	std::string name;

	bool operator <(const test_key& rhs) const {
		if (type != rhs.type) {
			return type < rhs.type;
		}
		if (name != rhs.name) {
			return name < rhs.name;
		}
		return false;
	}
};

struct test_result {
	std::string stream_name;
	std::string mixer_name;
	test_key key;
	statistic stats;
};

struct test_battery_result {
	using test_result_map = std::map<test_key, std::vector<test_result>>;

	std::string name;
	std::string mixer_name;
	test_result_map results;

	void add(const test_result& r) {
		results[r.key].push_back(r);
	}

	const std::vector<test_result>& operator[](test_key key) const {
		static const std::vector<test_result> empty;
		const auto it = results.find(key);
		return it != results.end() ? it->second : empty;
	}
};


template <typename T>
using mixer_test = std::function<sub_test_results(uint64_t n, const stream<T>&, const mixer<T>&)>;

template <typename T>
using stream_test = std::function<sub_test_results(uint64_t n, const stream<T>&)>;


inline sub_test_results split_test(const uint64_t n, const uint64_t max_size, const std::function<std::optional<statistic>(uint64_t)>& f) {
	if (n <= max_size) {
		return main_sub_test(f(n));
	}

	const auto split_count = std::max(2ull, n / max_size);
	sub_test_results results;
	for (uint64_t i = 0; i < split_count; ++i) {
		results.push_back({"s" + std::to_string(i + 1), f(max_size)});
	}
	return results;
}

///////////////////////////////////////////////////////////////
/// DATA TYPES
///////////////////////////////////////////////////////////////

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
