#pragma once

#include <iostream>

#include "util/table.h"
#include "util/statistic_analysis.h"

namespace mixer {

inline void draw_histogram(const std::vector<double>& data) {
	std::vector<uint64_t> bins(30);
	uint64_t max_count = 0;
	for (const auto r : rescale_to_01(data)) {
		const size_t index = std::min(static_cast<size_t>(r * bins.size()), bins.size() - 1);
		bins[index] ++;
		max_count = std::max(bins[index], max_count);
	}
	const double scale = std::min(1., 80. / static_cast<double>(max_count));
	for (const auto b : bins) {
		for (int i = 0; i < scale * b; ++i) {
			std::cout << "*";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

inline std::string to_string(const std::string& name, const double p_value) {
	std::stringstream ss;
	ss << " (p=" << p_value << + ") " << name;
	return ss.str();
}

inline std::string to_string(const test_key& key) {
	return get_test_name(key.type) + "-" + key.name;
}

inline void print_battery_result(const test_battery_result& battery_result) {
	table t({"KEY", "VALUE"});
	t.col("test subject").col(battery_result.test_subject_name).row();
	t.col("2^k").col(battery_result.power_of_two()).row();
	t.col("samples").col(battery_result.samples).row();
	t.col("n per sample").col(battery_result.n).row();
	t.col("-------").col("-------").row();
	for (const auto& e : battery_result.results) {
		{
			const auto& worst = get_worst_result(e.second);
			const auto analysis = statistic_analysis(worst.stats);
			if (analysis.has_suspicion()) {
				t.col(to_string(e.first));
				t.col(analysis.to_string() + to_string(worst.stream_name, worst.stats.p_value));
				t.row();
			}
		}

		{
			auto analysis = create_uniform_p_values_analysis(e.second);
			if (analysis.has_suspicion()) {
				t.col(to_string(e.first));
				t.col(analysis.to_string() + to_string("p-value uniformity", analysis.stat.p_value));
				t.row();
			}
		}
	}

	t.col("-------").col("-------").row();
	if (const auto analysis = get_worst_statistic_analysis(battery_result)) {
		t.col("SUMMARY").col((analysis->pass() ? "PASS: " : "FAIL: ") + analysis->to_string()).row();
	}

	if (battery_result.results.empty()) {
		t.col("SUMMARY").col("INCONCLUSIVE").row();
	}

	std::cout << t.to_string() << "\n";
}

}
