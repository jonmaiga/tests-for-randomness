#pragma once

#include "evaluate.h"
#include "format_result.h"
#include "search/mixer_constructions.h"
#include "util/test_setups.h"

namespace mixer {

inline void exhaust_command() {
	using T = uint32_t;

	constexpr int max_power = 27;
	const auto cb = [max_power](const test_battery_result& br) {
		const auto analysis = get_worst_statistic_analysis(br);
		if (!analysis) {
			return true;
		}
		return analysis->pass() && br.power_of_two() < max_power;
	};

	struct res {
		xm2x_constants<T> c;
		test_battery_result result;
	};

	std::vector<res> results;
	int total = 0;
	for (T c1 = 9; c1 <= 20; ++c1) {
		int sum = 0;
		std::cout << c1 << "-\n";
		for (T c2 = 9; c2 <= 20; ++c2) {
			for (T c3 = 9; c3 < 20; ++c3) {
				const xm2x_constants<T> c{c1, c2, c3, 2471660141};
				const auto mixer = create_xm2x_mixer<T>(c);
				const auto r = evaluate_multi_pass<T>(cb, create_mixer_test_setup<T>(mixer));
				results.push_back({c, r});
				sum += r.power_of_two();
				std::cout << r.power_of_two() << ";";
			}
			std::cout << "\n";
		}
		total += sum;
		std::cout << "sum: " << sum << "\n\n";
	}
	std::cout << "total: " << total << "\n\n";

	std::sort(results.begin(), results.end(), [](const res& a, const res& b) {
		return a.result.power_of_two() > b.result.power_of_two();
	});

	int i = 0;
	for (const auto& r : results) {
		std::cout << r.result.power_of_two() << "\n";
		if (++i > 20) break;
	}
}

}
