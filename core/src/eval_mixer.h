#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include "avalanche.h"
#include "mixers.h"
#include "rrc.h"
#include "streams.h"
#include "util/random.h"
#include "util/Table.h"


namespace mixer {

struct test_result {
	std::string name;
	std::vector<avalanche_result> results;
};

inline Table& col(Table& table, const avalanche_stats& stats) {
	return table.col(stats.std_dev_bias).
	             col(stats.mean_bias).
	             col(stats.max_bias);
}

inline void app(Table& table, const test_result& r) {
	avalanche_result worst;
	avalanche_result summed;
	for (const auto& rr : r.results) {
		if (rr.bic.max_bias > worst.bic.max_bias) {
			worst = rr;
		}
		summed.bic.max_bias += rr.bic.max_bias;
		summed.bic.mean_bias += rr.bic.mean_bias;
		summed.bic.std_dev_bias += rr.bic.std_dev_bias;

		summed.sac.max_bias += rr.sac.max_bias;
		summed.sac.mean_bias += rr.sac.mean_bias;
		summed.sac.std_dev_bias += rr.sac.std_dev_bias;

		summed.n += rr.n;
	}

	table.col(worst.mixer_name).col(worst.stream_name);
	col(table, worst.sac);
	col(table, worst.bic);
	table.col(worst.n).row();

	std::cout << table.to_string() << "\n";
}

struct test_config {
	stream stream;
	mixer mixer;
};

using test_factory = std::function<test_config()>;

inline test_result evaluate_rrc(const test_factory& factory) {
	test_result results{"rrc"};
	for (const rrc_type type : rrc_types) {
		for (int rot = 0; rot < 64; ++rot) {
			auto cfg = factory();
			const auto rrc_stream = add_rrc(cfg.stream, rot, type);
			const auto result = avalanche_bit_independence_test(rrc_stream, cfg.mixer);
			results.results.push_back({result});
		}
	}
	return results;
}

inline test_result evaluate_rrc(const mixer& mixer, uint64_t n) {
	const auto factory = [mixer, n]() {
		return test_config{create_counter_stream(1, n), mixer};
	};
	return evaluate_rrc(factory);
}


inline test_result evaluate(const test_factory& factory) {
	const auto cfg = factory();
	test_result result{"single"};
	const auto r = avalanche_bit_independence_test(cfg.stream, cfg.mixer);
	result.results.push_back({r});
	return result;
}

inline test_result evaluate(const mixer& mixer, uint64_t n) {
	const auto factory = [mixer, n]() {
		return test_config{create_counter_stream(1, n), mixer};
	};
	return evaluate(factory);
}

inline test_result evaluate_trng(uint64_t n) {
	auto data = readBinaryMustExist<uint64_t>(R"(C:\tmp\random.org\trng_small.bin)");
	const auto factory = [data, n]() {
		const auto dummy_stream = stream{
			"dummy", [n]() mutable -> std::optional<uint64_t> {
				if (n-- == 0) {
					throw std::runtime_error("No more stream data.");
				}
				return 1ull;
			}
		};

		std::size_t index = 0;
		const auto trng_mixer = mixer{
			"trng",
			[index, data](uint64_t) mutable-> uint64_t {
				if (index >= data.size()) {
					throw std::runtime_error("No more mixer data.");
				}
				return data[index++];
			}
		};
		return test_config{dummy_stream, trng_mixer};
	};

	return evaluate(factory);
}


}
