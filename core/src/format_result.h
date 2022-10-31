#pragma once

#include <iostream>
#include "util/table.h"
#include "statistics/tests.h"
#include "util/algo.h"

namespace mixer {

inline std::string p_value_test(const std::vector<result>& results) {

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
	const basic_stats b = compute_basic_stats(to_statistics(baseline[type]));
	const basic_stats r = compute_basic_stats(to_statistics(results[type]));
	return z_test(r.n, r.mean, b.mean);
}

inline double t_score(s_type type, const test_result& results, const test_result& baseline) {
	const basic_stats b = compute_basic_stats(to_statistics(baseline[type]));
	const basic_stats r = compute_basic_stats(to_statistics(results[type]));
	return t_test(r.n, r.mean, r.variance, b.n, b.mean, b.variance);
}

inline double f_score(s_type type, const test_result& results, const test_result& baseline) {
	const basic_stats b = compute_basic_stats(to_statistics(baseline[type]));
	const basic_stats r = compute_basic_stats(to_statistics(results[type]));
	return f_test(r.n, r.variance, b.n, b.variance);
}


inline double sum_abs(const std::vector<result>& rs) {
	double sum = 0;
	for (const auto& r : rs) {
		sum += std::abs(r.stats.value);
	}
	return sum;
}

inline double greatest_abs(const std::vector<result>& rs) {
	double max = -100000;
	for (const auto& r : rs) {
		max = std::max(std::abs(r.stats.value), max);
	}
	return max;
}


inline void draw_histogram(const std::vector<result>& results) {
	std::vector<uint64_t> bins(10);
	const auto s = to_unity(to_statistics(results));
	for (const auto r : s) {
		const size_t index = r * (bins.size() - 1);
		bins[index] ++;
	}
	for (const auto b : bins) {
		for (int i = 0; i < b; ++i) {
			std::cout << "*";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

class result_analyzer {

public:
	result_analyzer() :
		sample_table({
			"mixer",
			"mean-z", "variance-z",
			"chi2-tfz", "ks-z", "ad-z", "ww-z",
			"max_bias", "pearson", "spearman"
		}),
		p_table({
			"mixer",
			"mean-p", "variance-p", "chi2-p", "ks-p", "ad-p", "ww-p"
		}) {
	}

	void add(const test_result& r) {
		results.push_back(r);

		const auto b = results.front();
		//const basic_stats ww = compute_basic_stats(to_statistics(r[s_type::wald_wolfowitz]));
		sample_table
			.col(r.mixer_name)
			.col(z_score(s_type::basic_mean, r, b))
			//.col(z_score(s_type::basic_variance, r, b))
			.app(t_score(s_type::basic_variance, r, b)).app("/").app(f_score(s_type::basic_variance, r, b)).app("/").col(z_score(s_type::basic_variance, r, b))
			.app(t_score(s_type::chi2, r, b)).app("/").app(f_score(s_type::chi2, r, b)).app("/").col(z_score(s_type::chi2, r, b)) // not normal
			.col(z_score(s_type::kolmogorov_smirnov, r, b)) // normal?
			.col(z_score(s_type::anderson_darling, r, b)) // not normal
			.col(z_score(s_type::wald_wolfowitz_runs, r, b))
			.col(sum_abs(r[s_type::avalanche_max_bias]))
			.col(sum_abs(r[s_type::pearson_r]))
			.col(sum_abs(r[s_type::spearman_r]))
			.row();
		//std::cout << sample_table.to_string() << "\n";


		p_table
			.col(r.mixer_name)
			.col(p_value_test(r[s_type::basic_mean]))
			.col(p_value_test(r[s_type::basic_variance]))
			.col(p_value_test(r[s_type::chi2]))
			.col(p_value_test(r[s_type::kolmogorov_smirnov]))
			.col(p_value_test(r[s_type::anderson_darling]))
			.col(p_value_test(r[s_type::wald_wolfowitz_runs]))
			.row();

		std::cout << p_table.to_string() << "\n";

		//draw_histogram(r[s_type::kolmogorov_smirnov]);
		// Table full({"mixer", "stream", "ks"});
		// for (const auto& result : r.ks) {
		// 	full.col(result.mixer_name).col(result.stream_name).col(result.stats.d_max).row();
		// }
		// std::cout << full.to_string() << "\n";
	}

private:
	table sample_table;
	table p_table;
	std::vector<test_result> results;
};

}
