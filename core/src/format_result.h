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

inline std::string p_value_to_string(const double p_value) {
	std::stringstream ss;
	ss << "(p=" << p_value << + ")";
	return ss.str();
}

inline std::string to_string(const std::string& name, const double p_value) {
	return p_value_to_string(p_value) + " " + name;
}

inline std::string to_string(const test_key& key) {
	return get_test_name(key.type) + "-" + key.name;
}

struct result_analysis {
	std::string name;
	test_key key;
	statistic_analysis analysis;
	std::vector<test_result> results;

	std::string to_string() const {
		return p_value_to_string(analysis.stat.p_value) + " " + analysis.to_string() + " " + name;
	}

	bool operator <(const result_analysis& rhs) const {
		return get_comparable_p_value(analysis.stat) < get_comparable_p_value(rhs.analysis.stat);
	}
};

inline std::vector<result_analysis> filter_to_show(std::vector<result_analysis> results) {
	std::sort(results.begin(), results.end());

	std::map<test_type, std::vector<result_analysis>> per_test;
	for (const auto& e : results) {
		per_test[e.key.type].push_back(e);
	}

	std::vector<result_analysis> to_show;
	for (const auto& e : per_test) {
		const auto& ras = e.second;
		const auto suspicious_count = std::ranges::count_if(ras, [](const result_analysis& ra) { return ra.analysis.has_suspicion(); });
		const auto showing_count = std::min(suspicious_count, static_cast<int64_t>(3));
		const auto& worst = ras.front();
		if (worst.analysis.has_suspicion()) {
			int added = 0;
			for (const auto& m : ras) {
				if (m.analysis.has_suspicion()) {
					to_show.push_back(m);
					++added;
				}
				if (added == showing_count) {
					break;
				}
			}
		}
	}

	std::sort(to_show.begin(), to_show.end());
	return to_show;
}


inline void print_battery_result(const test_battery_result& battery_result) {
	table t({"KEY", "VALUE"});
	t.col("test subject").col(battery_result.test_subject_name).row();
	t.col("2^k").col(battery_result.power_of_two()).row();
	t.col("samples").col(battery_result.samples).row();
	t.col("n per sample").col(battery_result.n).row();
	t.col("-------").col("-------").row();

	// SAMPLE ANALYSIS
	std::vector<result_analysis> ras;
	for (const auto& e : battery_result.results) {
		const auto worst = get_worst_result(e.second);
		ras.push_back({worst.stream_name, e.first, statistic_analysis{worst.stats}, e.second});

		const auto analysis = create_uniform_p_values_analysis(e.second);
		const auto name = "meta analysis over " + std::to_string(e.second.size()) + " samples";
		ras.push_back({name, e.first, analysis, e.second});
	}
	std::sort(ras.begin(), ras.end());

	const auto to_show = filter_to_show(ras);
	for (const auto& ra : to_show) {
		t.col(to_string(ra.key));
		t.col(ra.to_string());
		t.row();
	}

	t.col("-------").col("-------").row();

	if (battery_result.results.empty()) {
		t.col("SUMMARY").col("INCONCLUSIVE").row();
	}
	else {
		const auto failures = std::ranges::count_if(ras, [](const result_analysis& r) { return !r.analysis.pass(); });
		const auto suspicious = std::ranges::count_if(ras, [](const result_analysis& r) { return r.analysis.has_suspicion(); }) - failures;
		const auto& ra = ras.front();
		t.col("SUMMARY")
		 .app(ra.analysis.pass() ? "PASSED" : "FAILED AT")
		 .app(" 2^")
		 .app(battery_result.power_of_two())
		 .app(" with ").app(suspicious).app(" suspicious and ").app(failures).app(" failures.").col()
		 .row();
	}

	std::cout << t.to_string() << "\n\n";
}

}
