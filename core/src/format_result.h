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
	for (const auto& rr : r.results) {
		if (std::abs(rr.bic.max_bias) > std::abs(worst.bic.max_bias)) {
			worst = rr;
		}
	}
	add_avalanche_result(table, worst);
}

inline void add_all(Table& table, const test_result& r) {
	auto rs = r.results;
	std::sort(rs.begin(), rs.end(), [](const avalanche_result& l, const avalanche_result& r) {
		return l.bic.max_bias < r.bic.max_bias;
	});

	for (const auto& rr : rs) {
		add_avalanche_result(table, rr);
	}
	std::cout << table.to_string() << "\n";
}

class result_analyzer {

public:
	result_analyzer() :
		runtime_table({"mixer", "stream", "sac_std_bias", "sac_avg_bias", "sac_max_bias", "bic_std_bias", "bic_avg_bias", "bic_max_bias", "n"}) {
	}

	void add(const test_result& r) {
		results.push_back(r);
		add_worst(runtime_table, r);
		std::cout << runtime_table.to_string() << "\n";
	}

	std::string summarize() const {
		std::vector<avalanche_result> rows;
		for (const auto& mixer_result : results) {
			auto rs = mixer_result.results;
			std::sort(rs.begin(), rs.end(), [](const avalanche_result& l, const avalanche_result& r) {
				return l.bic.max_bias < r.bic.max_bias;
			});
			rows.push_back(rs.back());
		}

		std::sort(rows.begin(), rows.end(), [](const avalanche_result& l, const avalanche_result& r) {
			return l.bic.max_bias < r.bic.max_bias;
		});

		Table table({"mixer", "stream", "sac_std_bias", "sac_avg_bias", "sac_max_bias", "bic_std_bias", "bic_avg_bias", "bic_max_bias", "n"});
		for (const auto& row : rows) {
			add_avalanche_result(table, row);
		}
		return table.to_string();
	}

private:
	Table runtime_table;
	std::vector<test_result> results;
};

}
