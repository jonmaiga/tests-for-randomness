#pragma once

#include <functional>
#include <string>
#include <vector>

#include "avalanche.h"
#include "kolmogorov.h"
#include "rrc.h"
#include "streams.h"
#include "tests.h"

namespace mixer {

template <typename T>
using test = const std::function<T(const stream&, const mixer&)>;

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
std::vector<T> evaluate_rrc(
	const std::function<T(const stream&, const mixer&)>& test,
	const std::vector<test_factory>& test_factories) {
	std::vector<T> results;
	for (const auto& factory : test_factories) {
		for (const rrc_type type : rrc_types) {
			for (int rot = 0; rot < 64; ++rot) {
				auto cfg = factory();
				const auto rrc_stream = add_rrc(cfg.stream, rot, type);
				results.push_back(test(rrc_stream, cfg.mixer));
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
		results.push_back(test(cfg.stream, cfg.mixer));
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

inline test_factory create_trng_factory(const std::vector<uint64_t>& data, size_t block, uint64_t n) {
	return [&data, n, block]() {
		const auto dummy_stream = stream{
			"block-" + std::to_string(block), [n]() mutable -> uint64_t {
				if (n-- == 0) {
					throw std::runtime_error("No more stream data.");
				}
				return 1ull;
			}
		};

		const auto trng_mixer = mixer{
			"trng",
			[index = block * avalanche_draws, &data](uint64_t) mutable-> uint64_t {
				if (index >= data.size()) {
					throw std::runtime_error("No more mixer data.");
				}
				return data[index++];
			}
		};
		return test_config{dummy_stream, trng_mixer};
	};
}

inline test_result evaluate_trng(uint64_t n) {
	const auto wanted_elements = n * avalanche_draws * 256;
	std::cout << "Reading trng stream from disk...";
	const auto data = readBinaryMustExist<uint64_t>(R"(C:\tmp\random.org\trng.bin)", wanted_elements);
	std::cout << " done!\n";
	if (data.size() < wanted_elements) {
		std::cout << "WARNING: not enough data, " << wanted_elements << "/" << data.size() << "\n";
	}
	std::vector<test_factory> factories;
	for (size_t block = 0; block < 256; ++block) {
		factories.push_back(create_trng_factory(data, block, n));
	}
	return evaluate(factories);
}


}
