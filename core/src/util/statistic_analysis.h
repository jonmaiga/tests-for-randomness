#pragma once

#include "statistics/kolmogorov.h"
#include "types.h"

#include <sstream>
#include <vector>

namespace tfr {

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

inline double get_comparable_p_value(statistic stat) {
	if (stat.type == statistic_type::z_score) {
		return stat.p_value;
	}
	return std::min(stat.p_value, 1. - stat.p_value);
}

inline test_result get_worst_result(const std::vector<test_result>& test_results) {
	std::optional<test_result> worst;
	for (const auto& tr : test_results) {
		const double p_value = get_comparable_p_value(tr.stats);
		if (!worst || p_value < get_comparable_p_value(worst->stats)) {
			worst = tr;
		}
	}
	return *worst;
}

class statistic_analysis {
public:
	explicit statistic_analysis(statistic stat) : stat(stat) {
	}

	bool has_remark() const {
		return get_failure_strength() > 0;
	}

	bool has_suspicion() const {
		return get_failure_strength() > 4;
	}

	bool pass() const {
		return get_failure_strength() < 9;
	}

	unsigned int get_failure_strength() const {
		const auto p = get_comparable_p_value(stat);
		if (p < 1e-10) {
			return 10;
		}
		const auto s = static_cast<unsigned int>(std::abs(std::log10(p)));
		assertion(s >= 0 && s <= 10, "Strength out of range");
		return s;
	}

	std::string to_string() const {
		static const std::vector<std::string> strings = {
			"no remarks",
			"minor(1)",
			"minor(2)",
			"minor(3)",
			"minor(4)",
			"suspicious(5)",
			"suspicious(6)",
			"very suspicious(7)",
			"very suspicious(8)",
			"failure(9)",
			"failure(10)",
		};
		return strings[get_failure_strength()];
	}

	double get_p_value_cmp() const {
		return get_comparable_p_value(stat);
	}

	statistic stat;
};

inline statistic_analysis create_uniform_p_values_analysis(const std::vector<test_result>& test_results) {
	return statistic_analysis(*kolmogorov_smirnov_stats(to_p_values(test_results)));
}

inline statistic_analysis create_statistic_analysis(const std::vector<test_result>& test_results) {
	return statistic_analysis(get_worst_result(test_results).stats);
}

inline std::string p_value_to_string(const double p_value) {
	std::stringstream ss;
	ss << p_value;
	return ss.str();
}

struct result_analysis {
	enum class type { Sample, Meta };

	type type;
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

inline std::vector<result_analysis> get_analysis(const test_battery_result& battery_result) {
	std::vector<result_analysis> ras;
	for (const auto& e : battery_result.results) {
		const auto worst = get_worst_result(e.second);
		ras.push_back({result_analysis::type::Sample, worst.stream_name, e.first, statistic_analysis{worst.stats}, e.second});

		const auto analysis = create_uniform_p_values_analysis(e.second);
		const auto name = "meta analysis over " + std::to_string(e.second.size()) + " samples";
		ras.push_back({result_analysis::type::Meta, name, e.first, analysis, e.second});
	}
	std::sort(ras.begin(), ras.end());
	return ras;
}

inline std::optional<statistic_analysis> get_worst_statistic_analysis(const test_battery_result& battery_result) {
	const auto ras = get_analysis(battery_result);
	if (ras.empty()) {
		return {};
	}
	return ras.front().analysis;
}


}
