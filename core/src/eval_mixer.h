#pragma once

#include <functional>
#include <string>
#include <vector>

#include "test/avalanche.h"
#include "test/basic.h"
#include "test/chi2.h"
#include "test/correlation.h"
#include "test/kolmogorov.h"
#include "rrc.h"
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

inline stream add_rrc(const stream& source, int rotation, rrc_type type) {
	const auto name = to_string(type) + "-" + std::to_string(rotation) + "(" + source.name + ")";
	return {
		name, [source, rotation, type]()-> uint64_t {
			return permute(source(), rotation, type);
		}
	};
}

template <typename T>
std::vector<T> evaluate_rrc(const test<T>& test, const std::vector<test_factory>& test_factories) {
	std::vector<T> results;
	for (const auto& factory : test_factories) {
		for (const rrc_type type : rrc_types) {
			for (int rot = 0; rot < 64; ++rot) {
				auto cfg = factory();
				const auto rrc_stream = add_rrc(cfg.stream, rot, type);
				results.push_back(test(cfg.n, rrc_stream, cfg.mixer));
			}
		}
	}
	return results;
}

inline test_result evaluate_rrc(const std::string& mixer_name, const std::vector<test_factory>& test_factories) {
	test_result results{"rrc", mixer_name};
	results.avalanche = evaluate_rrc<avalanche_result>(avalanche_test, test_factories);
	results.basic = evaluate_rrc<basic_result>(basic_test, test_factories);
	results.ks = evaluate_rrc<kolmogorov_result>(kolmogorov_test, test_factories);
	results.chi2 = evaluate_rrc<chi2_result>(chi2_test, test_factories);
	results.correlation = evaluate_rrc<correlation_result>(correlation_mixer_test, test_factories);
	return results;
}

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

inline test_result evaluate_rrc(const mixer& mixer, uint64_t n) {
	return internal::evaluate_rrc(mixer.name, create_test_factories(mixer, n));
}

inline test_result evaluate(const mixer& mixer, uint64_t n) {
	return internal::evaluate(mixer.name, create_test_factories(mixer, n));
}

}
