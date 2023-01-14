#pragma once

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

inline double get_comparable_p_value(statistic stat) {
	if (stat.type == statistic_type::z_score) {
		return stat.p_value;
	}
	return std::min(stat.p_value, 1. - stat.p_value);
}


class meta_analysis {
public:
	explicit meta_analysis(statistic stat) : stat(stat) {
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
			"minor (1)",
			"minor (2)",
			"minor (3)",
			"minor (4)",
			"suspicious (5)",
			"suspicious (6)",
			"very suspicious (7)",
			"very suspicious (8)",
			"failure (9)",
			"failure (10)",
		};
		return strings[get_failure_strength()];
	}

	statistic stat;
};

inline meta_analysis create_meta_analysis(const std::vector<test_result>& test_results) {
	//return meta_analysis(*kolmogorov_smirnov_stats(to_p_values(test_results)));
	const auto p_values = to_p_values(test_results);
	std::optional<statistic> worst;
	if (p_values.size() >= 32) {
		//worst = kolmogorov_smirnov_stats(p_values);
	}

	double worst_p_value = worst ? worst->p_value : 1000;
	for (const auto& tr : test_results) {
		const double p_value = get_comparable_p_value(tr.stats);
		if (p_value < worst_p_value) {
			worst = tr.stats;
			worst_p_value = p_value;
		}
	}
	return meta_analysis(*worst);
}

}
