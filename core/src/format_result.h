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
		statistics.push_back(r.stats.p_value);
	}
	return statistics;
}

inline uint64_t count_fails(const std::vector<double>& p_values, double alpha) {
	uint64_t fails = 0;
	for (const auto pv : p_values) {
		if (pv < alpha || pv > 1. - alpha) {
			fails += 1;
		}
	}
	return fails;
}

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

inline std::string p_value_test(const std::vector<result>& results) {
	if (results.empty()) {
		return "N/A";
	}

	// if (results.front().stats.type == s_type::chi2) {
	// 	const auto st = basic_stats(to_statistics(results));
	// 	draw_histogram(to_p_values(results));
	// 	draw_histogram(to_statistics(results));
	// 	std::cout << "stat mean: " << st.mean << " stat var: " << st.variance() << "\n";
	// }
	//const auto p_value = fishers_combined_probabilities(to_p_values(results));

	const auto p_values = to_p_values(results);
	const auto ks_stat = kolmogorov_smirnov_stats(p_values);
	const auto p_value = kolmogorov_smirnov_cdf(ks_stat, p_values.size() - 1, 100);
	constexpr auto a = 0.005;
	const auto fails = "(" + std::to_string(count_fails(p_values, a)) + ")";
	if (p_value < a || p_value > 1. - a) {
		return "F*" + fails;
	}
	return "P " + fails;
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
			"mean", "chi2", "ks", "ad", "ww", "pearson",
			"spearman", "kendall", "g1", "g2", "g3", "coupon", "d2", "d3",
			"sac", "bic"
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
			.col(p_value_test(r[s_type::kendall_tau]))
			.col(p_value_test(r[s_type::gap_low]))
			.col(p_value_test(r[s_type::gap_medium]))
			.col(p_value_test(r[s_type::gap_high]))
			.col(p_value_test(r[s_type::coupon]))
			.col(p_value_test(r[s_type::divisibility_2]))
			.col(p_value_test(r[s_type::divisibility_3]))
			.col(p_value_test(r[s_type::sac]))
			.col(p_value_test(r[s_type::bic]))
			.row();

		std::cout << p_table.to_string() << "\n";
	}

	std::vector<result> query(const tags& mixer_tags, const tags& stream_tags) const {
		auto results = find_by_mixer_name(_flatten(), mixer_tags);
		results = find_by_stream_name(results, stream_tags);
		std::sort(results.begin(), results.end(), [](const result& a, const result& b) {
			return a.stats.p_value < b.stats.p_value;
		});
		return results;
	}

	void list_results(const tags& mixer_tags, const tags& stream_tags) const {
		table t({"mixer", "stream", "statistic", "value", "p_value"});
		for (const auto& r : query(mixer_tags, stream_tags)) {
			if (r.stats.p_value > 0.05 && r.stats.p_value < 0.95) continue;
			t.col(r.mixer_name).
			  col(r.stream_name).
			  col(get_meta(r.stats.type).name).
			  col(r.stats.value).
			  col(r.stats.p_value).
			  row();
		}
		std::cout << t.to_string() << "\n";
	}

	void summarize_fails(const tags& stream_tags) const {
		tags mixer_tags;
		for (const auto& tr : test_results) {
			mixer_tags.push_back(tr.mixer_name);
		}
		summarize_fails(mixer_tags, stream_tags);
	}

	void summarize_fails(const tags& mixer_tags, const tags& stream_tags) const {
		// todo: clean this up
		std::vector<fail_summary> summaries;
		if (stream_tags.empty()) {
			for (const auto& mixer_tag : mixer_tags) {
				if (const auto s = get_fail_summary({mixer_tag}, {})) {
					summaries.push_back(*s);
				}
			}
		}
		else if (mixer_tags.empty()) {
			for (const auto& stream_tag : stream_tags) {
				if (const auto s = get_fail_summary({}, {stream_tag})) {
					summaries.push_back(*s);
				}
			}
		}
		else {
			for (const auto& mixer_tag : mixer_tags) {
				for (const auto& stream_tag : stream_tags) {
					if (const auto s = get_fail_summary({mixer_tag}, {stream_tag})) {
						summaries.push_back(*s);
					}
				}
			}
		}
		std::sort(summaries.begin(), summaries.end(), [](const fail_summary& a, const fail_summary& b) {
			return a.percent() < b.percent();
		});

		table t({"mixer", "stream", "total", "# fails", "% fails"});
		for (const auto& s : summaries) {
			t.col(s.mixer_tag).col(s.stream_tag).col(s.total).col(s.fails).col(s.percent()).row();
		}
		std::cout << t.to_string() << "\n";
	}


private:
	struct fail_summary {
		std::string mixer_tag;
		std::string stream_tag;
		double fails{};
		double total{};
		double percent() const { return fails / total; }
	};

	std::optional<fail_summary> get_fail_summary(const tags& mixer_tags, const tags& stream_tags) const {
		const auto& r = query(mixer_tags, stream_tags);
		if (r.empty()) return {};
		double fails = 0;
		for (const auto pv : to_p_values(r)) {
			constexpr auto a = 0.005;
			if (pv < a || pv > 1. - a) {
				fails += 1;
			}
		}
		const auto mixer_tag = mixer_tags.empty() ? "*" : join(mixer_tags, ",");
		const auto stream_tag = stream_tags.empty() ? "*" : join(stream_tags, ",");
		return fail_summary{mixer_tag, stream_tag, fails, static_cast<double>(r.size())};
	}


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
