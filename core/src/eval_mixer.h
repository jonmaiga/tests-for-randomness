#pragma once

#include <functional>
#include <string>
#include <vector>

#include "test/andersondarling.h"
#include "test/avalanche.h"
#include "test/basic.h"
#include "test/chi2.h"
#include "test/correlation.h"
#include "test/kolmogorov.h"
#include "test/waldwolfowitz.h"
#include "test_streams.h"

namespace mixer {

template <typename T>
using test = const std::function<T(uint64_t n, const stream&, const mixer&)>;

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
std::vector<result<T>> evaluate(const test<T>& test, const std::vector<test_factory>& test_factories) {
	std::vector<result<T>> results;
	for (const auto& factory : test_factories) {
		const auto cfg = factory();
		const auto r = test(cfg.n, cfg.stream, cfg.mixer);
		results.push_back({cfg.stream.name, cfg.mixer.name, r});
	}
	return results;
}

inline test_result evaluate(const std::string& mixer_name, const std::vector<test_factory>& test_factories) {
	test_result result{"single", mixer_name};
	result.basic = evaluate<basic_stats>(basic_test, test_factories);
	result.chi2 = evaluate<chi2_stats>(chi2_test, test_factories);
	result.ks = evaluate<kolmogorov_stats>(kolmogorov_test, test_factories);
	result.anderson_darling = evaluate<anderson_darling_stats>(anderson_darling_test, test_factories);
	result.ww = evaluate<wald_wolfowitz_stats>(wald_wolfowitz_test, test_factories);
	result.avalanche = evaluate<avalanche_stats>(avalanche_mixer_test, test_factories);
	result.correlation = evaluate<correlation_stats>(correlation_mixer_test, test_factories);
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
