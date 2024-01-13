#pragma once

#include <iostream>

#include "prng.h"
#include "util/table.h"
#include "util/statistic_analysis.h"

namespace tfr {
template <typename T>
void draw_histogram(const T& data) {
	std::vector<uint64_t> bins(30);
	uint64_t max_count = 0;
	for (const auto r : data) {
		assertion(is_valid_between_01(r), "invalid or out of range histogram value, rescale first.");
		const size_t index = std::min(static_cast<size_t>(r * bins.size()), bins.size() - 1);
		++bins[index];
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

inline void draw_histogram_rescale(const std::vector<double>& data) {
	draw_histogram(rescale_to_01(data));
}

inline std::string to_string(const test_key& key) {
	return get_test_name(key.type) + "-" + key.name;
}

inline std::string to_string(const seed_data& seed) {
	return "[" + std::to_string(seed.s64()) + ", ...]";
}

inline std::vector<result_analysis> filter_to_show(std::vector<result_analysis> results) {
	std::sort(results.begin(), results.end());

	std::map<test_type, std::vector<result_analysis>> per_test;
	for (const auto& e : results) {
		per_test[e.key.type].push_back(e);
	}

	std::vector<result_analysis> to_show;
	for (const auto& e : per_test) {
		const auto& ras = e.second;
		const uint64_t suspicious_count = std::count_if(ras.begin(), ras.end(), [](const result_analysis& ra) { return ra.analysis.has_suspicion(); });
		const uint64_t showing_count = std::min(suspicious_count, static_cast<uint64_t>(3));
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

inline std::vector<std::string> get_tests(const test_battery_result& br) {
	std::set<std::string> test_names;
	for (const auto& result : br.results) {
		test_names.insert(get_test_name(result.first.type));
	}
	return std::vector(test_names.begin(), test_names.end());
}

inline std::vector<std::string> get_failed_tests(const std::vector<result_analysis>& ras) {
	std::set<std::string> test_names;
	for (const auto& ra : ras) {
		if (!ra.analysis.pass()) {
			test_names.insert(get_test_name(ra.key.type));
		}
	}
	return std::vector(test_names.begin(), test_names.end());
}

inline void print_battery_result(const test_battery_result& battery_result) {
	std::cout << "==========================================================================================\n";
	std::cout
		<< battery_result.test_subject_name
		<< " 2^" << battery_result.power_of_two()
		<< " with " << battery_result.samples << " samples using "
		<< battery_result.bits << "-bits (" << battery_result.passed_milliseconds / 1000 << "s)\n";
	std::cout << join(get_tests(battery_result), ",") << "\n";
	std::cout << "==========================================================================================\n";

	const auto ras = get_analysis(battery_result);
	table tests_table({"test", "p-value", "remark", "stream/description"});
	for (const auto& ra : filter_to_show(ras)) {
		tests_table.col(to_string(ra.key))
		           .col(p_value_to_string(ra.analysis.stat.p_value))
		           .col(ra.analysis.to_string())
		           .col(ra.name)
		           .row();
	}
	std::cout << tests_table.to_string() << "\n";

	if (battery_result.results.empty()) {
		std::cout << "INCONCLUSIVE\n";
		std::cout << "\n";
	}
	else {
		const auto failures = std::count_if(ras.begin(), ras.end(), [](const result_analysis& r) { return !r.analysis.pass(); });
		const auto suspicious = std::count_if(ras.begin(), ras.end(), [](const result_analysis& r) { return r.analysis.has_suspicion(); }) - failures;
		const auto& ra = ras.front();
		std::cout << (ra.analysis.pass() ? "PASSED" : "***FAILED*** at");
		std::cout << " 2^" << battery_result.power_of_two() << " with a total of ";
		std::cout << failures << " failures and " << suspicious << " suspicious results.\n";
		std::cout << "\n";
	}
}
}
