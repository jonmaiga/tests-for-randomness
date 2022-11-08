#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "test_streams.h"
#include "statistics/andersondarling.h"
#include "statistics/avalanche.h"
#include "statistics/basic.h"
#include "statistics/chi2.h"
#include "statistics/correlation.h"
#include "statistics/kolmogorov.h"
#include "statistics/waldwolfowitz.h"

namespace mixer {

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
			assertion(is_valid_normal(r.stats.p_value), "a p-value is not valid or normal");
			results[r.stats.type].push_back(r);
		}
	}

	const std::vector<result>& operator[](s_type type) const {
		static const std::vector<result> empty;
		const auto it = results.find(type);
		return it != results.end() ? it->second : empty;
	}
};

namespace internal {

inline stream create_stream(const test_config& cfg) {
	auto s = create_stream_from_mixer(cfg.source, cfg.mixer);
	if (cfg.append_stream_factory) {
		return cfg.append_stream_factory(s);
	}
	return s;
}

inline std::vector<result> evaluate_mixer(const mixer_test& test, const std::vector<test_factory>& test_factories) {
	std::vector<result> results;
	for (const auto& factory : test_factories) {
		const auto cfg = factory();
		for (const auto& r : test(cfg.n, cfg.source, cfg.mixer)) {
			results.push_back({cfg.source.name, cfg.mixer.name, r});
		}
	}
	return results;
}

inline std::vector<result> evaluate_stream(const stream_test& test, const std::vector<test_factory>& test_factories) {
	std::vector<result> results;
	for (const auto& factory : test_factories) {
		const auto cfg = factory();
		const auto s = create_stream(cfg);
		for (const auto& r : test(cfg.n, s)) {
			results.push_back({s.name, cfg.mixer.name, r});
		}
	}
	return results;
}

inline test_result evaluate(const std::string& mixer_name, const std::vector<test_factory>& test_factories) {
	test_result result{"single", mixer_name};

	result.add(evaluate_stream(basic_test, test_factories));
	result.add(evaluate_stream(chi2_test, test_factories));
	result.add(evaluate_stream(kolmogorov_test, test_factories));
	result.add(evaluate_stream(anderson_darling_test, test_factories));
	result.add(evaluate_stream(wald_wolfowitz_test, test_factories));

	result.add(evaluate_mixer(avalanche_mixer_sac_test, test_factories));
	result.add(evaluate_mixer(avalanche_mixer_bic_test, test_factories));
	result.add(evaluate_mixer(pearson_correlation_mixer_test, test_factories));
	result.add(evaluate_mixer(spearman_correlation_mixer_test, test_factories));
	//result.add(evaluate_mixer(kendall_correlation_mixer_test, test_factories));
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
