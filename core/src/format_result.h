#pragma once

#include <iostream>
#include "util/Table.h"

namespace mixer {

inline double get_worst(const avalanche_result& result) { return result.bic.max_bias; }
inline double get_worst(const basic_result& result) { return result.stats.mean; }
inline double get_worst(const chi2_result& result) { return result.stats.chi2; }
inline double get_worst(const kolmogorov_result& result) { return result.stats.d_max; }
inline double get_worst(const correlation_result& result) { return result.stats.kendall_t; }

template <typename T>
T get_worst(const std::vector<T>& results) {
	T worst = results.front();
	for (const auto& result : results) {
		if (get_worst(result) > get_worst(worst)) {
			worst = result;
		}
	}
	return worst;
}

inline avalanche_result& operator+=(avalanche_result& l, const avalanche_result& r) {
	l.n += r.n;
	l.sac.max_bias += r.sac.max_bias;
	l.sac.mean_bias += r.sac.mean_bias;
	l.sac.std_dev_bias += r.sac.std_dev_bias;
	l.bic.max_bias += r.bic.max_bias;
	l.bic.mean_bias += r.bic.mean_bias;
	l.bic.std_dev_bias += r.bic.std_dev_bias;
	return l;
}

inline basic_result& operator+=(basic_result& l, const basic_result& r) {
	l.n += r.n;
	l.stats.mean += r.stats.mean;
	l.stats.variance += r.stats.variance;
	l.stats.median += r.stats.median;
	return l;
}

inline kolmogorov_result& operator+=(kolmogorov_result& l, const kolmogorov_result& r) {
	l.n += r.n;
	l.stats.d_max += r.stats.d_max;
	l.stats.i_max += r.stats.i_max;
	return l;
}

inline chi2_result& operator+=(chi2_result& l, const chi2_result& r) {
	l.n += r.n;
	l.stats.chi2 += r.stats.chi2;
	return l;
}

inline correlation_result& operator+=(correlation_result& l, const correlation_result& r) {
	l.n += r.n;
	l.stats.pearson_r += std::abs(r.stats.pearson_r);
	l.stats.spearman_r += std::abs(r.stats.spearman_r);
	l.stats.kendall_t += std::abs(r.stats.kendall_t);
	return l;
}

template <typename T>
T get_sum(const std::vector<T>& results) {
	T sum{"-", results.front().mixer_name};
	for (const auto& result : results) {
		sum += result;
	}
	return sum;
}

template <typename T>
void sort_rows(std::vector<T>& rows, const std::function<double(const T&)>& field) {
	std::sort(rows.begin(), rows.end(), [field](const T& l, const T& r) {
		return field(l) < field(r);
	});
}

inline Table& add_avalanche_stats(Table& table, const avalanche_stats& stats) {
	return table.col(stats.std_dev_bias).col(stats.mean_bias).col(stats.max_bias);
}

class result_analyzer {

public:
	result_analyzer() :
		runtime_table({"mixer", "mean", "bic_max", "ks_max", "chi2", "pearson_r", "spearman_r", "kendall_t"}) {
	}

	void add(const test_result& r) {
		results.push_back(r);
		const auto aw = get_sum(r.avalanche_results);
		const auto bs = get_sum(r.basic_results);
		const auto kw = get_sum(r.ks_results);
		const auto ch = get_sum(r.chi2_results);
		const auto pr = get_worst(r.correlation_results);
		runtime_table
			.col(r.mixer_name)
			.col(bs.stats.mean)
			.col(aw.bic.max_bias)
			.col(kw.stats.d_max)
			.col(ch.stats.chi2)
			.col(pr.stats.pearson_r)
			.col(pr.stats.spearman_r)
			.col(pr.stats.kendall_t)
			.row();
		std::cout << runtime_table.to_string() << "\n";
	}

	std::string summarize_avalanche() const {
		std::vector<avalanche_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.avalanche_results));
		}

		sort_rows<avalanche_result>(rows, [](const avalanche_result& r) { return r.bic.max_bias; });

		Table table({
			"mixer", "stream", "n",
			"sac_std_bias", "sac_avg_bias", "sac_max_bias",
			"bic_std_bias", "bic_avg_bias", "bic_max_bias"
		});
		for (const auto& row : rows) {
			table.col(row.mixer_name).col(row.stream_name).col(row.n);
			add_avalanche_stats(table, row.sac);
			add_avalanche_stats(table, row.bic);
			table.row();
		}
		return table.to_string();
	}

	std::string summarize_basic() const {
		std::vector<basic_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.basic_results));
		}
		sort_rows<basic_result>(rows, [](const basic_result& r) { return std::abs(r.stats.mean - 0.5); });

		Table table({"mixer", "stream", "n", "mean", "variance", "median"});
		for (const auto& row : rows) {
			table
				.col(row.mixer_name)
				.col(row.stream_name)
				.col(row.n)
				.col(row.stats.mean)
				.col(row.stats.variance)
				.col(row.stats.median)
				.row();
		}
		return table.to_string();
	}


	std::string summarize_ks() const {
		std::vector<kolmogorov_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.ks_results));
		}
		sort_rows<kolmogorov_result>(rows, [](const kolmogorov_result& r) { return r.stats.d_max; });

		Table table({"mixer", "stream", "n", "d", "index"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
			     .col(row.n)
			     .col(row.stats.d_max)
			     .col(row.stats.i_max)
			     .row();
		}
		return table.to_string();
	}

	std::string summarize_chi2() const {
		std::vector<chi2_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.chi2_results));
		}
		sort_rows<chi2_result>(rows, [](const chi2_result& r) { return r.stats.chi2; });

		Table table({"mixer", "stream", "n", "chi2"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
			     .col(row.n)
			     .col(row.stats.chi2)
			     .row();
		}
		return table.to_string();
	}

	std::string summarize_correlation() const {
		std::vector<correlation_result> rows;
		for (const auto& result : results) {
			rows.push_back(get_sum(result.correlation_results));
		}
		sort_rows<correlation_result>(rows, [](const correlation_result& r) { return r.stats.pearson_r; });

		Table table({"mixer", "stream", "n", "pearson_r", "spearman_r", "kendall_t"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
			     .col(row.n)
			     .col(row.stats.pearson_r)
			     .col(row.stats.spearman_r)
			     .col(row.stats.kendall_t)
			     .row();
		}
		return table.to_string();
	}

	std::string summarize_baseline_bias() const {
		struct columns {
			std::string name;
			double mean{};
			double variance{};
			double chi2{};
			double ks{};
			double avalanche{};
			double pearson_r{};
			double spearman_r{};
			double kendall_t{};
		};

		const auto get_columns = [](const test_result& r) {
			columns c;
			c.name = r.mixer_name;
			c.mean = get_sum(r.basic_results).stats.mean;
			c.variance = get_sum(r.basic_results).stats.variance;
			c.chi2 = get_sum(r.chi2_results).stats.chi2;
			c.ks = get_sum(r.ks_results).stats.d_max;
			c.avalanche = get_sum(r.avalanche_results).bic.max_bias;
			c.pearson_r = get_sum(r.correlation_results).stats.pearson_r;
			c.spearman_r = get_sum(r.correlation_results).stats.spearman_r;
			c.kendall_t = get_sum(r.correlation_results).stats.kendall_t;
			return c;
		};

		const auto& baseline = get_columns(results.front());
		std::vector<columns> rows;
		for (const auto& result : results) {
			const auto& cols = get_columns(result);
			rows.push_back({
				cols.name,
				cols.mean / baseline.mean,
				cols.variance / baseline.variance,
				cols.chi2 / baseline.chi2,
				cols.ks / baseline.ks,
				cols.avalanche / baseline.avalanche,
				cols.pearson_r / baseline.pearson_r,
				cols.spearman_r / baseline.spearman_r,
				cols.kendall_t / baseline.kendall_t,
			});
		}

		sort_rows<columns>(rows, [](const columns& row) { return row.pearson_r; });

		Table table({
			"mixer", "mean", "variance", "chi2", "ks", "avalanche",
			"pearson_r", "spearman_r", "kendall_t"
		});
		for (const auto& row : rows) {
			table.col(row.name)
			     .col(row.mean)
			     .col(row.variance)
			     .col(row.chi2)
			     .col(row.ks)
			     .col(row.avalanche)
			     .col(row.pearson_r)
			     .col(row.spearman_r)
			     .col(row.kendall_t)
			     .row();
		}
		return table.to_string();
	}


private:
	Table runtime_table;
	std::vector<test_result> results;
};

}
