#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "mixer.h"
#include "stream.h"
#include "util/assertion.h"
#include "util/math.h"

namespace tfr {

template <typename T>
using stream_factory = std::function<stream<T>()>;

template <typename T>
using streams = std::vector<stream<T>>;


///////////////////////////////////////////////////////////////
/// STATISTIC TYPES
///////////////////////////////////////////////////////////////

enum class statistic_type {
	z_score,
	chi2,
	kolmogorov_smirnov_d,
	anderson_darling_A2,
};

enum class test_type {
	mean,
	uniform,
	wald_wolfowitz_runs,
	bit_count_2d,
	gap,
	coupon,
	divisibility,
	permutation,

	// mixer
	sac,
	bic,
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
	uint64_t n{};
	test_key key;
	statistic stats;
};

struct test_battery_result {
	using test_result_map = std::map<test_key, std::vector<test_result>>;

	std::string test_subject_name;
	uint64_t n{};
	uint64_t samples{};
	int bits{};
	test_result_map results;
	uint64_t passed_milliseconds{};

	void add(const test_result& r) {
		results[r.key].push_back(r);
	}

	const std::vector<test_result>& operator[](const test_key& key) const {
		static const std::vector<test_result> empty;
		const auto it = results.find(key);
		return it != results.end() ? it->second : empty;
	}

	int power_of_two() const {
		return static_cast<int>(std::ceil(std::log2(n)));
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
	const auto split_count = std::max(static_cast<uint64_t>(2), n / max_size);
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

inline unsigned int default_max_threads() {
	return std::max(std::thread::hardware_concurrency() - 4, 2u);
}

}
