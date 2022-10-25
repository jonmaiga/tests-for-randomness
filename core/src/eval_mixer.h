#pragma once

#include <functional>
#include <string>
#include <vector>

#include "avalanche.h"
#include "kolmogorov.h"
#include "rrc.h"
#include "streams.h"
#include "test_streams.h"

namespace mixer {

template <typename T>
using test = const std::function<T(uint64_t n, const stream&, const mixer&)>;

struct test_result {
	std::string name;
	std::vector<avalanche_result> avalanche_results;
	std::vector<kolmogorov_result> ks_results;
};

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

inline test_result evaluate_rrc(const std::vector<test_factory>& test_factories) {
	test_result results{"rrc"};
	results.avalanche_results = evaluate_rrc<avalanche_result>(avalanche_test, test_factories);
	results.ks_results = evaluate_rrc<kolmogorov_result>(kolmogorov_test, test_factories);
	return results;
}

inline test_result evaluate_rrc(const mixer& mixer, uint64_t n) {
	return evaluate_rrc(create_test_streams(mixer, n));
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

inline test_result evaluate(const std::vector<test_factory>& test_factories) {
	test_result result{"single"};
	result.avalanche_results = evaluate<avalanche_result>(avalanche_test, test_factories);
	result.ks_results = evaluate<kolmogorov_result>(kolmogorov_test, test_factories);
	return result;
}

inline test_result evaluate(const mixer& mixer, uint64_t n) {
	return evaluate(create_test_streams(mixer, n));
}

inline test_result evaluate_trng(uint64_t n) {
	const auto shared_stream = create_data_stream("trng", get_trng_data());

	const auto trng_mixer = mixer{
		"trng",
		[&shared_stream](uint64_t) -> uint64_t {
			return shared_stream();
		}
	};

	const auto factory = [&]()-> test_config {
		return {n, shared_stream, trng_mixer};
	};

	std::vector<test_factory> factories;
	for (size_t block = 0; block < 1024; ++block) {
		factories.emplace_back(factory);
	}
	return evaluate(factories);
}


}
