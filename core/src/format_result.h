#pragma once

#include <iostream>

#include "test_definitions.h"
#include "util/table.h"
#include "statistics/fishersmethod.h"
#include "util/algo.h"

namespace mixer {

inline std::vector<double> to_statistics(const std::vector<test_result>& results) {
	std::vector<double> statistics;
	statistics.reserve(results.size());
	for (const auto& r : results) {
		statistics.push_back(r.stats.value);
	}
	return statistics;
}

inline std::vector<double> to_p_values(const std::vector<test_result>& results) {
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

inline bool passed_test(const std::vector<test_result>& results, const double alpha) {
	const auto p_values = to_p_values(results);
	const auto ks_stat = kolmogorov_smirnov_stats(p_values);
	const auto p_value = ks_stat->p_value;
	return p_value >= alpha && p_value <= 1. - alpha;
}

using tags = std::vector<std::string>;

inline std::vector<test_result> find_by_mixer_name(const std::vector<test_result>& results, const tags& mixer_names) {
	if (mixer_names.empty()) {
		return results;
	}
	std::vector<test_result> found;
	for (const auto& result : results) {
		for (const auto& tag : mixer_names) {
			if (result.mixer_name == tag) {
				found.push_back(result);
			}
		}
	}
	return found;
}

inline std::vector<test_result> find_by_stream_name(const std::vector<test_result>& results, const tags& stream_tags) {
	if (stream_tags.empty()) {
		return results;
	}
	std::vector<test_result> found;
	for (const auto& result : results) {
		for (const auto& stream_tag : stream_tags) {
			if (contains(result.stream_name, stream_tag)) {
				found.push_back(result);
			}
		}
	}
	return found;
}

class meta_analysis {
public:
	explicit meta_analysis(const std::vector<double>& p_values) :
		stat(kolmogorov_smirnov_stats(p_values)) {
	}

	bool has_remark() const {
		return get_failure_strength() > 0;
	}

	bool has_suspicion() const {
		return get_failure_strength() > 2;
	}

	bool pass() const {
		return get_failure_strength() < 5;
	}

	unsigned int get_failure_strength() const {
		if (!stat) {
			return 0;
		}
		const double lower_tail = (1. - 2. * std::abs(stat->p_value - 0.5));
		if (lower_tail < 1e-10) {
			return 10;
		}
		const auto s = static_cast<unsigned int>(std::abs(std::log10(lower_tail)));
		assertion(s >= 0 && s <= 10, "Strength out of range");
		return s;
	}

	std::string to_string() const {
		if (!stat) {
			return "NA";
		}
		static const std::vector<std::string> strings = {
			"pass",
			"minor (1)",
			"minor (2)",
			"suspicious (3)",
			"suspicious (4)",
			"failure (5)",
			"failure (6)",
			"failure (7)",
			"failure (8)",
			"failure (9)",
			"failure (10)",
		};
		return strings[get_failure_strength()];
	}

	std::optional<statistic> stat;
};

inline std::optional<meta_analysis> get_meta_analysis(const test_battery_result& battery_result) {
	std::optional<meta_analysis> worst;
	for (const auto& e : battery_result.results) {
		const auto interpretation = meta_analysis(to_p_values(e.second));
		if (!worst || worst->get_failure_strength() < interpretation.get_failure_strength()) {
			worst = interpretation;
		}
	}
	return worst;
}

struct result_analysis {
	test_key key;
	std::vector<test_result> results;
	meta_analysis analysis;
};

inline std::vector<result_analysis> get_result_analysis(const test_battery_result& battery_result) {
	std::vector<result_analysis> all;
	for (const auto& e : battery_result.results) {
		all.push_back({e.first, e.second, meta_analysis(to_p_values(e.second))});
	}
	return all;
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
		const auto& p_values = to_p_values(e.second);
		const auto interpretation = meta_analysis(p_values);
		if (interpretation.has_suspicion()) {
			t.col(to_string(e.first));
			t.col(interpretation.to_string());
			t.row();
		}
	}

	t.col("-------").col("-------").row();
	if (const auto meta = get_meta_analysis(battery_result)) {
		t.col("SUMMARY").col(meta->to_string()).row();
	}

	std::cout << t.to_string() << "\n";
}

class result_analyzer {

public:
	void add(const test_battery_result& battery_result) {
		test_results.push_back(battery_result);
	}

	std::vector<test_result> query(const tags& mixer_tags, const tags& stream_tags) const {
		auto results = find_by_mixer_name(_flatten(), mixer_tags);
		results = find_by_stream_name(results, stream_tags);
		std::sort(results.begin(), results.end(), [](const test_result& a, const test_result& b) {
			return a.stats.p_value < b.stats.p_value;
		});
		return results;
	}

	void list_results(const tags& mixer_tags, const tags& stream_tags) const {
		table t({"mixer", "stream", "n", "statistic", "value", "p_value", "df"});
		for (const auto& r : query(mixer_tags, stream_tags)) {
			if (r.stats.p_value > 0.05 && r.stats.p_value < 0.95) {
				continue;
			}
			t.col(r.mixer_name).
			  col(r.stream_name).
			  col(r.n).
			  col(get_test_name(r.key.type)).
			  col(r.stats.value).
			  col(r.stats.p_value).
			  col(r.stats.df).
			  row();
		}
		std::cout << t.to_string() << "\n";
	}

	void summarize_fails(const tags& stream_tags) const {
		tags mixer_tags;
		for (const auto& tr : test_results) {
			mixer_tags.push_back(tr.test_subject_name);
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
		if (r.empty()) {
			return {};
		}
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

	std::vector<test_result> _flatten() const {
		std::vector<test_result> rs;
		for (const auto& ts : test_results) {
			for (const auto& r : ts.results) {
				append(rs, r.second);
			}
		}
		return rs;
	}

	std::vector<test_battery_result> test_results;
};

}
