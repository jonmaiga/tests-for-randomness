#pragma once

#include <iostream>
#include "util/table.h"
#include "statistics/t_test.h"

namespace mixer {

template <typename T>
void sort_rows(std::vector<T>& rows, const std::function<double(const T&)>& field) {
	std::sort(rows.begin(), rows.end(), [field](const T& l, const T& r) {
		return field(l) < field(r);
	});
}

inline table& add_avalanche_stats(table& table, const bias& bias) {
	return table.col(bias.std_dev_bias).col(bias.mean_bias).col(bias.max_bias);
}

template <typename T>
std::string p_value_test(const std::vector<T>& results) {

	int fails = 0;
	for (const auto& r : results) {
		if (r.stats.p_value < 0.005) {
			++fails;
		}
	}
	return fails == 0 ? "OK" : "FAIL:" + std::to_string(fails);
}

inline std::vector<double> to_statistics(const std::vector<result>& results) {
	std::vector<double> statistics;
	statistics.reserve(results.size());
	for (const auto& r : results) {
		statistics.push_back(r.stats.value);
	}
	return statistics;
}

inline double z_score(s_type type, const test_result& results, const test_result& baseline) {
	const basic_stats r = compute_basic_stats(to_statistics(results[type]));
	const basic_stats b = compute_basic_stats(to_statistics(baseline[type]));
	return z_test(r, b);
}

class result_analyzer {

public:
	result_analyzer() :
		test_table({
			"mixer",
			"basic-mean-z", "basic-variance-z",
			"chi2-z", "ks-z", "ad-z", "ww-z",
			"max_bias-z", "pearson-z"
		}) {
	}

	void add(const test_result& r) {
		results.push_back(r);

		const auto baseline = results.front();
		test_table
			.col(r.mixer_name)
			.col(z_score(s_type::basic_mean, r, baseline))
			.col(z_score(s_type::chi2, r, baseline))
			.col(z_score(s_type::kolmogorov_smirnov, r, baseline))
			.col(z_score(s_type::anderson_darling, r, baseline))
			.col(z_score(s_type::wald_wolfowitz, r, baseline))
			.col(z_score(s_type::avalanche_max_bias, r, baseline))
			.col(z_score(s_type::pearson_r, r, baseline))
			.row();
		std::cout << test_table.to_string() << "\n";

		// Table full({"mixer", "stream", "ks"});
		// for (const auto& result : r.ks) {
		// 	full.col(result.mixer_name).col(result.stream_name).col(result.stats.d_max).row();
		// }
		// std::cout << full.to_string() << "\n";
	}

private:
	table test_table;
	std::vector<test_result> results;
};

}
