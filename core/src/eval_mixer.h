#pragma once

#include <functional>
#include <string>
#include <vector>

#include "statistics/andersondarling.h"
#include "statistics/avalanche.h"
#include "statistics/basic.h"
#include "statistics/chi2.h"
#include "statistics/correlation.h"
#include "statistics/kolmogorov.h"
#include "statistics/waldwolfowitz.h"
#include "test_streams.h"

namespace mixer {

template <typename T>
using mixer_test = const std::function<T(uint64_t n, const stream&, const mixer&)>;

template <typename T>
using stream_test = const std::function<T(uint64_t n, const stream&)>;

template <typename T>
struct result {
	std::string stream_name;
	std::string mixer_name;
	T stats;
};

struct test_result {
	std::string name;
	std::string mixer_name;

	std::vector<result<basic_stats>> basic;
	std::vector<result<chi2_stats>> chi2;
	std::vector<result<kolmogorov_stats>> ks;
	std::vector<result<anderson_darling_stats>> anderson_darling;
	std::vector<result<wald_wolfowitz_stats>> ww;

	std::vector<result<avalanche_stats>> avalanche;
	std::vector<result<correlation_stats>> correlation;
};

namespace internal {

template <typename T>
std::vector<result<T>> evaluate_mixer(const mixer_test<T>& test, const std::vector<test_factory>& test_factories) {
	std::vector<result<T>> results;
	for (const auto& factory : test_factories) {
		const auto cfg = factory();
		const auto r = test(cfg.n, cfg.source, cfg.mixer);
		results.push_back({cfg.source.name, cfg.mixer.name, r});
	}
	return results;
}

inline stream create_stream(const test_config& cfg) {
	auto s = create_stream_from_mixer(cfg.source, cfg.mixer);
	if (cfg.append_stream_factory) {
		return cfg.append_stream_factory(s);
	}
	return s;
}

template <typename T>
std::vector<result<T>> evaluate_stream(const stream_test<T>& test, const std::vector<test_factory>& test_factories) {
	std::vector<result<T>> results;
	for (const auto& factory : test_factories) {
		const auto cfg = factory();
		const auto s = create_stream(cfg);
		const auto r = test(cfg.n, s);
		results.push_back({s.name, cfg.mixer.name, r});
	}
	return results;
}


inline test_result evaluate(const std::string& mixer_name, const std::vector<test_factory>& test_factories) {
	test_result result{"single", mixer_name};

	result.basic = evaluate_stream<basic_stats>(basic_test, test_factories);
	result.chi2 = evaluate_stream<chi2_stats>(chi2_test, test_factories);
	result.ks = evaluate_stream<kolmogorov_stats>(kolmogorov_test, test_factories);
	result.anderson_darling = evaluate_stream<anderson_darling_stats>(anderson_darling_test, test_factories);
	result.ww = evaluate_stream<wald_wolfowitz_stats>(wald_wolfowitz_test, test_factories);

	result.avalanche = evaluate_mixer<avalanche_stats>(avalanche_mixer_test, test_factories);
	result.correlation = evaluate_mixer<correlation_stats>(correlation_mixer_test, test_factories);
	return result;
}

} // namespace internal

inline test_result test_standard(const mixer& mixer, uint64_t n) {
	return internal::evaluate(mixer.name, create_test_factories(mixer, n));
}

inline test_result test_rrc(const mixer& mixer, uint64_t n) {
	return internal::evaluate(mixer.name, create_rrc_test_factories(mixer, n));
}

// test_mixer  => rrc and/or standard
// test_prng   => seed with rrc and/or standard
// test_stream => standard only

}
