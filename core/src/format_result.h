#pragma once

#include <iostream>
#include "util/table.h"
#include "statistics/tests.h"
#include "statistics/fishersmethod.h"

namespace mixer {

inline std::vector<double> to_statistics(const std::vector<result>& results) {
	std::vector<double> statistics;
	statistics.reserve(results.size());
	for (const auto& r : results) {
		statistics.push_back(r.stats.value);
	}
	return statistics;
}

inline std::vector<double> to_p_values(const std::vector<result>& results) {
	std::vector<double> statistics;
	statistics.reserve(results.size());
	for (const auto& r : results) {
		if (auto p_value = r.stats.p_value) {
			statistics.push_back(*p_value);
		}
	}
	return statistics;
}

inline void draw_histogram(const std::vector<double>& data) {
	std::vector<uint64_t> bins(30);
	uint64_t max_count = 0;
	for (const auto r : normalize_to_uniform(data)) {
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

inline std::string p_value_test(const std::vector<result>& results) {
	if (results.empty()) {
		return "N/A";
	}

	const auto st = compute_basic_stats(to_statistics(results));
	if (results.front().stats.type == s_type::avalanche_sac) {
		// for (auto v : to_statistics(results)) {
		// 	std::cout << v << ", ";
		// }
		//std::cout << "\n\n";

		const auto st = compute_basic_stats(to_statistics(results));
		draw_histogram(to_p_values(results));
		draw_histogram(to_statistics(results));
		std::cout << "stat mean: " << st.mean << " stat var: " << st.variance << "\n";
	}

	const auto p_value = fishers_combined_probabilities(to_p_values(results));
	if (p_value < 0.005) {
		return "FAIL:" + std::to_string(st.mean);
	}
	return "OK: " + std::to_string(st.mean);
}


inline double z_score(s_type type, const test_result& results, const test_result& baseline) {
	const basic_stats b = compute_basic_stats(to_statistics(baseline[type]));
	const basic_stats r = compute_basic_stats(to_statistics(results[type]));
	return z_test(r.n, r.mean, b.mean, b.variance);
}

inline double t_score(s_type type, const test_result& results, const test_result& baseline) {
	const basic_stats b = compute_basic_stats(to_statistics(baseline[type]));
	const basic_stats r = compute_basic_stats(to_statistics(results[type]));
	return t_test(r.n, r.mean, r.variance, b.mean, b.variance);
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
			"mean-p", "chi2-p", "ks-p", "ad-p", "ww-p", "r-p", "s-p", "sac", "a-m-p"
		}) {
	}

	void add(const test_result& r) {
		results.push_back(r);

		const auto b = results.front();
		//const basic_stats ww = compute_basic_stats(to_statistics(r[s_type::wald_wolfowitz]));
		/*
		sample_table
			.col(r.mixer_name)
			.col(z_score(s_type::basic_mean, r, b))
			//.col(z_score(s_type::basic_variance, r, b))
			.app(t_score(s_type::basic_variance, r, b)).app("/").app(f_score(s_type::basic_variance, r, b)).app("/").col(z_score(s_type::basic_variance, r, b))
			.app(t_score(s_type::chi2, r, b)).app("/").app(f_score(s_type::chi2, r, b)).app("/").col(z_score(s_type::chi2, r, b)) // not normal
			.col(z_score(s_type::kolmogorov_smirnov, r, b)) // normal?
			.col(z_score(s_type::anderson_darling, r, b)) // not normal
			.col(z_score(s_type::wald_wolfowitz_runs, r, b))
			.col(sum_abs(r[s_type::avalanche_bic]))
			.col(sum_abs(r[s_type::pearson_r]))
			.col(sum_abs(r[s_type::spearman_r]))
			.row();*/
		//std::cout << sample_table.to_string() << "\n";


		p_table
			.col(r.mixer_name)
			.col(p_value_test(r[s_type::basic_mean]))
			.col(p_value_test(r[s_type::chi2]))
			.col(p_value_test(r[s_type::kolmogorov_smirnov]))
			.col(p_value_test(r[s_type::anderson_darling]))
			.col(p_value_test(r[s_type::wald_wolfowitz_runs]))
			.col(p_value_test(r[s_type::pearson_r]))
			.col(p_value_test(r[s_type::spearman_r]))
			.col(p_value_test(r[s_type::avalanche_sac]))
			.col(p_value_test(r[s_type::avalanche_bic]))
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
