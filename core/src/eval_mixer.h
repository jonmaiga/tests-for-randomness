#pragma once

#include <functional>
#include <string>
#include <vector>

#include "test/avalanche.h"
#include "test/basic.h"
#include "test/chi2.h"
#include "test/correlation.h"
#include "test/kolmogorov.h"
#include "test_streams.h"

namespace mixer {

template <typename T>
using test = const std::function<T(uint64_t n, const stream&, const mixer&)>;

struct test_result {
	std::string name;
	std::string mixer_name;
	std::vector<avalanche_result> avalanche;
	std::vector<basic_result> basic;
	std::vector<kolmogorov_result> ks;
	std::vector<chi2_result> chi2;
	std::vector<correlation_result> correlation;
};

namespace internal {

template <typename T>
std::vector<T> evaluate(const test<T>& test, const std::vector<test_factory>& test_factories) {
	std::vector<T> results;
	for (const auto& factory : test_factories) {
		const auto cfg = factory();
		results.push_back(test(cfg.n, cfg.stream, cfg.mixer));
	}
	return results;
}

inline test_result evaluate(const std::string& mixer_name, const std::vector<test_factory>& test_factories) {
	test_result result{"single", mixer_name};
	result.avalanche = evaluate<avalanche_result>(avalanche_test, test_factories);
	result.ks = evaluate<kolmogorov_result>(kolmogorov_test, test_factories);
	result.chi2 = evaluate<chi2_result>(chi2_test, test_factories);
	result.basic = evaluate<basic_result>(basic_test, test_factories);
	result.correlation = evaluate<correlation_result>(correlation_mixer_test, test_factories);
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
