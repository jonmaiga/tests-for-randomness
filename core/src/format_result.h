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
		if (rr.bic.max_bias > worst.bic.max_bias) {
			worst = rr;
		}
	}
	add_avalanche_result(table, worst);
	std::cout << table.to_string() << "\n";
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


}
