#pragma once

#include <iostream>
#include "util/Table.h"

namespace mixer {

inline double get_worst(const avalanche_stats& result) { return result.bic.max_bias; }
inline double get_worst(const basic_stats& result) { return result.mean; }
inline double get_worst(const chi2_stats& result) { return result.chi2; }
inline double get_worst(const kolmogorov_stats& result) { return result.d_max; }
inline double get_worst(const correlation_stats& result) { return result.kendall_t; }

template <typename T>
T get_worst(const std::vector<result<T>>& results) {
	T worst = results.front().stats;
	for (const auto& result : results) {
		if (get_worst(result.stats) > get_worst(worst)) {
			worst = result.stats;
		}
	}
	return worst;
}

inline avalanche_stats& operator+=(avalanche_stats& l, const avalanche_stats& r) {
	l.sac.max_bias += r.sac.max_bias;
	l.sac.mean_bias += r.sac.mean_bias;
	l.sac.std_dev_bias += r.sac.std_dev_bias;
	l.bic.max_bias += r.bic.max_bias;
	l.bic.mean_bias += r.bic.mean_bias;
	l.bic.std_dev_bias += r.bic.std_dev_bias;
	return l;
}

inline basic_stats& operator+=(basic_stats& l, const basic_stats& r) {
	l.mean += r.mean;
	l.variance += r.variance;
	l.median += r.median;
	return l;
}

inline kolmogorov_stats& operator+=(kolmogorov_stats& l, const kolmogorov_stats& r) {
	l.d_max += r.d_max;
	l.i_max += r.i_max;
	return l;
}

inline chi2_stats& operator+=(chi2_stats& l, const chi2_stats& r) {
	l.chi2 += r.chi2;
	return l;
}

inline correlation_stats& operator+=(correlation_stats& l, const correlation_stats& r) {
	l.pearson_r += std::abs(r.pearson_r);
	l.spearman_r += std::abs(r.spearman_r);
	l.kendall_t += std::abs(r.kendall_t);
	return l;
}

template <typename T>
T get_sum(const std::vector<result<T>>& results) {
	T sum;
	for (const auto& result : results) {
		sum += result.stats;
	}
	return sum;
}

template <typename T>
void sort_rows(std::vector<T>& rows, const std::function<double(const T&)>& field) {
	std::sort(rows.begin(), rows.end(), [field](const T& l, const T& r) {
		return field(l) < field(r);
	});
}

inline Table& add_avalanche_stats(Table& table, const bias& bias) {
	return table.col(bias.std_dev_bias).col(bias.mean_bias).col(bias.max_bias);
}

class result_analyzer {

public:
	result_analyzer() :
		runtime_table({"mixer", "mean", "bic_max", "ks_max", "chi2", "pearson_r", "spearman_r", "kendall_t"}) {
	}

	void add(const test_result& r) {
		results.push_back(r);
		const auto aw = get_sum(r.avalanche);
		const auto bs = get_sum(r.basic);
		const auto kw = get_sum(r.ks);
		const auto ch = get_sum(r.chi2);
		const auto pr = get_worst(r.correlation);
		runtime_table
			.col(r.mixer_name)
			.col(bs.mean)
			.col(aw.bic.max_bias)
			.col(kw.d_max)
			.col(ch.chi2)
			.col(pr.pearson_r)
			.col(pr.spearman_r)
			.col(pr.kendall_t)
			.row();
		std::cout << runtime_table.to_string() << "\n";
	}

	std::string summarize_avalanche() const {
		using T = result<avalanche_stats>;
		std::vector<T> rows;
		for (const auto& result : results) {
			rows.push_back({result.mixer_name, "-", get_sum(result.avalanche)});
		}

		sort_rows<T>(rows, [](const T& r) { return r.stats.bic.max_bias; });

		Table table({
			"mixer", "stream",
			"sac_std_bias", "sac_avg_bias", "sac_max_bias",
			"bic_std_bias", "bic_avg_bias", "bic_max_bias"
		});
		for (const auto& row : rows) {
			table.col(row.mixer_name).col(row.stream_name);
			add_avalanche_stats(table, row.stats.sac);
			add_avalanche_stats(table, row.stats.bic);
			table.row();
		}
		return table.to_string();
	}

	std::string summarize_basic() const {
		using T = result<basic_stats>;
		std::vector<T> rows;
		for (const auto& result : results) {
			rows.push_back({result.mixer_name, "-", get_sum(result.basic)});
		}
		sort_rows<T>(rows, [](const T& r) { return std::abs(r.stats.mean - 0.5); });

		Table table({"mixer", "stream", "mean", "variance", "median"});
		for (const auto& row : rows) {
			table
				.col(row.mixer_name)
				.col(row.stream_name)
				.col(row.stats.mean)
				.col(row.stats.variance)
				.col(row.stats.median)
				.row();
		}
		return table.to_string();
	}


	std::string summarize_ks() const {
		using T = result<kolmogorov_stats>;
		std::vector<T> rows;
		for (const auto& result : results) {
			rows.push_back({result.mixer_name, "-", get_sum(result.ks)});
		}
		sort_rows<T>(rows, [](const T& r) { return r.stats.d_max; });

		Table table({"mixer", "stream", "d", "index"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
			     .col(row.stats.d_max)
			     .col(row.stats.i_max)
			     .row();
		}
		return table.to_string();
	}

	std::string summarize_chi2() const {
		using T = result<chi2_stats>;
		std::vector<T> rows;
		for (const auto& result : results) {
			rows.push_back({result.mixer_name, "-", get_sum(result.chi2)});
		}
		sort_rows<T>(rows, [](const T& r) { return r.stats.chi2; });

		Table table({"mixer", "stream", "chi2"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
			     .col(row.stats.chi2)
			     .row();
		}
		return table.to_string();
	}

	std::string summarize_correlation() const {
		using T = result<correlation_stats>;
		std::vector<T> rows;
		for (const auto& result : results) {
			rows.push_back({result.mixer_name, "-", get_sum(result.correlation)});
		}
		sort_rows<T>(rows, [](const T& r) { return r.stats.pearson_r; });

		Table table({"mixer", "stream", "pearson_r", "spearman_r", "kendall_t"});
		for (const auto& row : rows) {
			table.col(row.mixer_name)
			     .col(row.stream_name)
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
			c.mean = get_sum(r.basic).mean;
			c.variance = get_sum(r.basic).variance;
			c.chi2 = get_sum(r.chi2).chi2;
			c.ks = get_sum(r.ks).d_max;
			c.avalanche = get_sum(r.avalanche).bic.max_bias;
			c.pearson_r = get_sum(r.correlation).pearson_r;
			c.spearman_r = get_sum(r.correlation).spearman_r;
			c.kendall_t = get_sum(r.correlation).kendall_t;
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
