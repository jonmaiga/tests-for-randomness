#pragma once

#include <iostream>
#include "util/table.h"
#include "statistics/fishersmethod.h"
#include "util/algo.h"

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

	if (results.front().stats.type == s_type::avalanche_sac) {
		const auto st = basic_stats(to_statistics(results));
		draw_histogram(to_p_values(results));
		draw_histogram(to_statistics(results));
		std::cout << "stat mean: " << st.mean << " stat var: " << st.variance() << "\n";
	}
	const auto p_value = fishers_combined_probabilities(to_p_values(results));
	constexpr auto a = 0.005;
	if (p_value < a || p_value > 1. - a) {
		return "!!:" + std::to_string(p_value);
	}
	return "OK: " + std::to_string(p_value);
}

using tags = std::vector<std::string>;

inline std::vector<result> find_by_mixer_name(const std::vector<result>& results, const tags& mixer_names) {
	if (mixer_names.empty()) {
		return results;
	}
	std::vector<result> found;
	for (const auto& result : results) {
		for (const auto& tag : mixer_names) {
			if (contains(result.mixer_name, tag)) {
				found.push_back(result);
			}
		}
	}
	return found;
}

inline std::vector<result> find_by_stream_name(const std::vector<result>& results, const tags& stream_tags) {
	if (stream_tags.empty()) {
		return results;
	}
	std::vector<result> found;
	for (const auto& result : results) {
		for (const auto& stream_tag : stream_tags) {
			if (contains(result.stream_name, stream_tag)) {
				found.push_back(result);
			}
		}
	}
	return found;
}


class result_analyzer {

public:
	result_analyzer() :
		p_table({
			"mixer",
			"mean", "chi2", "ks", "ad", "ww", "pearson", "spearman", "sac", "bic"
		}) {
	}

	void add(const test_result& r) {
		test_results.push_back(r);
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
	}

	void summarize(const tags& mixer_tags, const tags& stream_tags) const {
		auto results = find_by_mixer_name(_flatten(), mixer_tags);
		results = find_by_stream_name(results, stream_tags);
		std::sort(results.begin(), results.end(), [](const result& a, const result& b) {
			if (a.stats.p_value && b.stats.p_value) {
				return *a.stats.p_value < *b.stats.p_value;
			}
			return a.stats.value < b.stats.value;
		});
		table t({"mixer", "stream", "stat", "p_value"});
		for (const auto& r : results) {
			t.col(r.mixer_name).
			  col(r.stream_name).
			  col(r.stats.value);
			if (const auto pv = r.stats.p_value) {
				t.col(*pv);
			}
			else {
				t.col(std::string("NA"));
			}
			t.row();
		}

		std::cout << t.to_string() << "\n";
	}

private:
	std::vector<result> _flatten() const {
		std::vector<result> rs;
		for (const auto& ts : test_results) {
			for (const auto& r : ts.results) {
				append(rs, r.second);
			}
		}
		return rs;
	}

	table p_table;
	std::vector<test_result> test_results;
};

}
