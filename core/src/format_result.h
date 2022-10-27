#pragma once

#include <iostream>
#include "util/Table.h"

namespace mixer {

inline Table& add_avalanche_stats(Table& table, const avalanche_stats& stats) {
	return table.col(stats.std_dev_bias).
	             col(stats.mean_bias).
	             col(stats.max_bias);
}

inline Table& add_avalanche_result(Table& table, const avalanche_result& result) {
	table.col(result.mixer_name).col(result.stream_name);
	add_avalanche_stats(table, result.sac);
	add_avalanche_stats(table, result.bic);
	return table.col(result.n).row();
}

inline void add_worst(Table& table, const test_result& r) {
	avalanche_result worst;
	for (const auto& rr : r.avalanche_results) {
		if (std::abs(rr.bic.max_bias) > std::abs(worst.bic.max_bias)) {
			worst = rr;
		}
	}
	add_avalanche_result(table, worst);
}

inline void add_all(Table& table, const test_result& r) {
	auto rs = r.avalanche_results;
	std::sort(rs.begin(), rs.end(), [](const avalanche_result& l, const avalanche_result& r) {
		return l.bic.max_bias < r.bic.max_bias;
	});

	for (const auto& rr : rs) {
		add_avalanche_result(table, rr);
	}
	std::cout << table.to_string() << "\n";
}

inline avalanche_stats max(const avalanche_stats& l, const avalanche_stats& r) {
	avalanche_stats as;
	as.max_bias = std::max(l.max_bias, r.max_bias);
	as.std_dev_bias = std::max(l.std_dev_bias, r.std_dev_bias);
	if (std::abs(l.mean_bias) > std::abs(r.mean_bias)) {
		as.mean_bias = l.mean_bias;
	}
	else {
		as.mean_bias = r.mean_bias;
	}
	return as;
}

inline avalanche_result get_worst(const std::vector<avalanche_result>& results) {
	avalanche_result worst{"-"};
	for (const auto& rr : results) {
		if (rr.bic.max_bias > worst.bic.max_bias) {
			worst = rr;
		}
	}
	return worst;
}

inline basic_result get_worst(const std::vector<basic_result>& results) {
	basic_result worst;
	double worstDev = 0;
	for (const auto& rr : results) {
		const double dev = std::abs(rr.stats.mean - 0.5);
		if (dev > worstDev) {
			worst = rr;
			worstDev = dev;
		}
	}
	return worst;
}


inline kolmogorov_result get_worst(const std::vector<kolmogorov_result>& results) {
	kolmogorov_result worst{};
	for (const auto& rr : results) {
		if (rr.stats.d_max > worst.stats.d_max) {
			worst = rr;
		}
	}
	return worst;
}

inline chi2_result get_worst(const std::vector<chi2_result>& results) {
	chi2_result worst{};
	for (const auto& rr : results) {
		if (rr.stats.chi2 > worst.stats.chi2) {
			worst = rr;
		}
	}
	return worst;
}

inline basic_result get_sum(const std::vector<basic_result>& results) {
	basic_result sum{"-"};
	for (const auto& rr : results) {
		sum.mixer_name = rr.mixer_name;
		sum.stats.mean += rr.stats.mean;
		sum.stats.variance += rr.stats.variance;
		sum.stats.median += rr.stats.median;
		sum.stats.n += rr.stats.n;
	}
	return sum;
}


inline avalanche_result get_sum(const std::vector<avalanche_result>& results) {
	avalanche_result sum{"-"};
	for (const auto& result : results) {
		sum.mixer_name = result.mixer_name;
		sum.n += result.n;
		sum.sac.max_bias += result.sac.max_bias;
		sum.sac.mean_bias += result.sac.mean_bias;
		sum.sac.std_dev_bias += result.sac.std_dev_bias;
		sum.bic.max_bias += result.bic.max_bias;
		sum.bic.mean_bias += result.bic.mean_bias;
		sum.bic.std_dev_bias += result.bic.std_dev_bias;
	}
	return sum;
}

inline kolmogorov_result get_sum(const std::vector<kolmogorov_result>& results) {
	kolmogorov_result sum{"-"};
	for (const auto& rr : results) {
		sum.mixer_name = rr.mixer_name;
		sum.stats.d_max += rr.stats.d_max;
		sum.stats.i_max += rr.stats.i_max;
		sum.n += rr.n;
	}
	return sum;
}

inline chi2_result get_sum(const std::vector<chi2_result>& results) {
	chi2_result sum{"-"};
	for (const auto& rr : results) {
		sum.mixer_name = rr.mixer_name;
		sum.stats.chi2 += rr.stats.chi2;
		sum.n += rr.n;
	}
	return sum;
}


inline uint64_t sum_total_n(const test_result& r) {
	uint64_t total = 0;
	for (const auto& ar : r.avalanche_results) {
		total += ar.n;
	}
	for (const auto& ksr : r.ks_results) {
		total += ksr.n;
	}
	return total;
}


class result_analyzer {

public:
	result_analyzer() :
		runtime_table({"mixer", "mean", "bic_max", "ks_max", "chi2"}) {
	}

	void add(const test_result& r) {
		results.push_back(r);
		const auto aw = get_sum(r.avalanche_results);
		const auto bs = get_sum(r.basic_results);
		const auto kw = get_sum(r.ks_results);
		const auto ch = get_worst(r.chi2_results);
		runtime_table
			.col(aw.mixer_name)
			.col(bs.stats.mean)
			.col(aw.bic.max_bias)
			.col(kw.stats.d_max)
			.col(ch.stats.chi2)
			.row();
		std::cout << runtime_table.to_string() << "\n";
	}

	std::string summarize_avalanche() const {
		std::vector<avalanche_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.avalanche_results));
		}

		std::sort(rows.begin(), rows.end(), [](const avalanche_result& l, const avalanche_result& r) {
			return l.bic.max_bias < r.bic.max_bias;
		});

		Table table({
			"mixer", "stream",
			"sac_std_bias", "sac_avg_bias", "sac_max_bias",
			"bic_std_bias", "bic_avg_bias", "bic_max_bias", "n"
		});
		for (const auto& row : rows) {
			add_avalanche_result(table, row);
		}
		return table.to_string();
	}

	std::string summarize_basic() const {
		std::vector<basic_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.basic_results));
		}

		std::sort(rows.begin(), rows.end(), [](const basic_result& l, const basic_result& r) {
			return std::abs(l.stats.mean - 0.5) < std::abs(r.stats.mean - 0.5);
		});

		Table table({"mixer", "stream", "mean", "variance", "median", "n"});
		for (const auto& row : rows) {
			table
				.col(row.mixer_name)
				.col(row.stream_name)
				.col(row.stats.mean)
				.col(row.stats.variance)
				.col(row.stats.median)
				.col(row.stats.n)
				.row();
		}
		return table.to_string();
	}


	std::string summarize_ks() const {
		std::vector<kolmogorov_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.ks_results));
		}

		std::sort(rows.begin(), rows.end(), [](const kolmogorov_result& l, const kolmogorov_result& r) {
			return l.stats.d_max < r.stats.d_max;
		});

		Table table({"mixer", "stream", "d", "index", "n"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
			     .col(row.stats.d_max)
			     .col(row.stats.i_max)
			     .col(row.n).row();
		}
		return table.to_string();
	}

	std::string summarize_chi2() const {
		std::vector<chi2_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.chi2_results));
		}

		std::sort(rows.begin(), rows.end(), [](const chi2_result& l, const chi2_result& r) {
			return l.stats.chi2 < r.stats.chi2;
		});

		Table table({"mixer", "stream", "chi2", "n"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
			     .col(row.stats.chi2)
			     .col(row.n).row();
		}
		return table.to_string();
	}


private:
	Table runtime_table;
	std::vector<test_result> results;
};

}
